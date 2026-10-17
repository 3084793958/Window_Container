#include "window_container.h"
#include <QMessageBox>
#include <QGuiApplication>
#include <QCoreApplication>
#include "public_config.h"

Window_Picker::Window_Picker(Display *dpy, QObject *parent)
    : QObject(parent)
    , m_display(dpy)
{}

Window_Picker::~Window_Picker()
{
    if (m_notifier)
    {
        m_notifier->setEnabled(false);
        m_notifier->deleteLater();
        m_notifier = nullptr;
    }
    if (m_display)
    {
        XUngrabPointer(m_display, CurrentTime);
        XUngrabKeyboard(m_display, CurrentTime);
        if (m_cursor)
        {
            XFreeCursor(m_display, m_cursor);
            m_cursor = 0;
        }
    }
}

void Window_Picker::start(std::function<void(bool, Window)> callback)
{
    m_callback = callback;
    if (!m_display)
    {
        finish(false, 0);
        return;
    }
    int screen = DefaultScreen(m_display);
    Window root = RootWindow(m_display, screen);
    m_cursor = XCreateFontCursor(m_display, XC_crosshair);
    if (!m_cursor)
    {
        finish(false, 0);
        return;
    }
    int grab = XGrabPointer(m_display, root, False, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, 0L, m_cursor, CurrentTime);
    if (grab != GrabSuccess)
    {
        finish(false, 0);
        return;
    }
    XGrabKeyboard(m_display, root, False, GrabModeAsync, GrabModeAsync, CurrentTime);
    XSync(m_display, False);

    int xfd = ConnectionNumber(m_display);
    m_notifier = new QSocketNotifier(xfd, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, [this]()
    {
        while (XPending(m_display) > 0)
        {
            XEvent ev;
            XNextEvent(m_display, &ev);
            if (ev.type == ButtonPress)
            {
                Window target = ev.xbutton.subwindow;
                if (target == 0L)
                {
                    target = RootWindow(m_display, DefaultScreen(m_display));
                }
                Window client = getClientWindow(m_display, target);
                finish(true, client);
                return;
            }
            else if (ev.type == 2)
            {
                KeySym keysym = XLookupKeysym(&ev.xkey, 0);
                if (keysym == XK_Escape)
                {
                    finish(false, 0);
                    return;
                }
            }
        }
    });
}

void Window_Picker::finish(bool success, Window win)
{
    if (m_notifier)
    {
        m_notifier->setEnabled(false);
        m_notifier->deleteLater();
        m_notifier = nullptr;
    }
    if (m_display)
    {
        XUngrabPointer(m_display, CurrentTime);
        if (m_cursor)
        {
            XFreeCursor(m_display, m_cursor);
            m_cursor = 0;
        }
    }
    if (m_callback)
    {
        m_callback(success, win);
    }
    this->deleteLater();
}

Window Window_Picker::tryFindClientWindow(Display *dpy, Window win, Atom wmStateAtom)
{
    Window root, parent;
    Window *children;
    unsigned int nchildren;
    if (!XQueryTree(dpy, win, &root, &parent, &children, &nchildren))
    {
        return 0;
    }
    Window result = 0;
    for (unsigned int i = 0; !result && i < nchildren; i++)
    {
        Atom actualType;
        int actualFormat;
        unsigned long nitems, bytesAfter;
        unsigned char *data = nullptr;
        XGetWindowProperty(dpy, children[i], wmStateAtom, 0, 0, False, AnyPropertyType, &actualType, &actualFormat, &nitems, &bytesAfter, &data);
        if (data)
        {
            XFree(data);
        }
        if (actualType != 0L)
        {
            result = children[i];
        }
    }
    for (unsigned int i = 0; !result && i < nchildren; i++)
    {
        result = tryFindClientWindow(dpy, children[i], wmStateAtom);
    }
    if (children)
    {
        XFree(children);
    }
    return result;
}

Window Window_Picker::getClientWindow(Display *dpy, Window win)
{
    Atom wmStateAtom = XInternAtom(dpy, "WM_STATE", True);
    if (wmStateAtom == 0L)
    {
        return win;
    }
    Atom actualType;
    int actualFormat;
    unsigned long nitems, bytesAfter;
    unsigned char *data = nullptr;
    XGetWindowProperty(dpy, win, wmStateAtom, 0, 0, False, AnyPropertyType, &actualType, &actualFormat, &nitems, &bytesAfter, &data);
    if (data)
    {
        XFree(data);
    }
    if (actualType != 0L)
    {
        return win;
    }
    Window result = tryFindClientWindow(dpy, win, wmStateAtom);
    return result ? result : win;
}

Window_Container::Window_Container(QObject *parent)
    : QObject(parent)
{
    m_is_wayland = QGuiApplication::platformName().startsWith(QStringLiteral("wayland"));
    connect(m_timer, &QTimer::timeout, this, &Window_Container::checkWindowStatus);
    m_timer->setInterval(1000);
    m_timer->start();
    connect(item_widget, &Plugin_Item_Widget::Add_Window, this, &Window_Container::selectWindow);
    connect(item_widget, &Plugin_Item_Widget::remove_window, this, [=]
    {
        pluginStateSwitched();
        pluginStateSwitched();
    });
    item_widget->move(0, 0);
}

Window_Container::~Window_Container()
{
    pluginUnload();
}

bool Window_Container::pluginIsAllowUnload()
{
    return true;
}

bool Window_Container::pluginIsAllowDisable()
{
    return true;
}

bool Window_Container::pluginIsDisable()
{
    if (m_proxyInter)
    {
        return m_proxyInter->getValue(this, "disabled", false).toBool();
    }
    else
    {
        m_timer->stop();
        return true;
    }
}

void Window_Container::pluginStateSwitched()
{
    const bool disabledNew = !pluginIsDisable();
    if (disabledNew)
    {
        m_timer->stop();
    }
    else
    {
        m_timer->start();
    }
    if (m_proxyInter)
    {
        m_proxyInter->saveValue(this, "disabled", disabledNew);
    }
    if (disabledNew)
    {
        if (m_proxyInter)
        {
            m_proxyInter->itemRemoved(this, pluginName());
        }
    }
    else
    {
        if (m_proxyInter)
        {
            m_proxyInter->itemAdded(this, pluginName());
        }
    }
}

const QString Window_Container::pluginName() const
{
    return QStringLiteral("Window_Container");
}

const QString Window_Container::pluginDisplayName() const
{
    return QString("Window_Container");
}

void Window_Container::pluginUnload()
{
    if (m_picker)
    {
        m_picker->deleteLater();
        m_picker = nullptr;
    }
    m_timer->stop();
    disconnect();
    window_name_label->deleteLater();
    item_widget->deleteLater();
    if (m_display)
    {
        XCloseDisplay(m_display);
        m_display = nullptr;
    }
}

void Window_Container::positionChanged(const Dock::Position position)
{
    plugin_pos = static_cast<int>(position);
    checkWindowStatus();
    if (update_sender)
    {
        update_sender->Send();
    }
}

QSize Window_Container::pluginSetWidgetSize(PluginsItemInterface::Carrier_Type carrier_type, bool *isValid)
{
    switch (carrier_type)
    {
    case PluginsItemInterface::Carrier_Type::Unknown:
    {
        *isValid = false;
        return QSize();
    }
    case PluginsItemInterface::Carrier_Type::Plugin_Item:
    {
        *isValid = true;
        return QSize();
    }
    case PluginsItemInterface::Carrier_Type::Plugin_Tips:
    {
        *isValid = true;
        return QSize();
    }
    case PluginsItemInterface::Carrier_Type::Plugin_Popup:
    {
        *isValid = false;
        return QSize();
    }
    }
    *isValid = false;
    return QSize();
}

void Window_Container::pluginGetSender(P_Sender * const m_update_sender, P_Sender * const m_send_data_sender)
{
    update_sender = m_update_sender;
    (void) m_send_data_sender;
}

QPoint Window_Container::pluginSetCarrierPos(PluginsItemInterface::Carrier_Type carrier_type, QPoint item_pos, QSize item_size, bool *isValid)
{
    if (Carrier_Type::Plugin_Tips == carrier_type)
    {
        *isValid = true;
        if (item_widget->parentWidget() && window_name_label->parentWidget())
        {
            QPoint mouse_pos = item_widget->parentWidget()->mapFromGlobal(QCursor::pos());
            if (plugin_pos == 0)
            {
                return item_pos + QPoint(mouse_pos.x() - window_name_label->parentWidget()->width() / 2, item_size.height() + 5);
            }
            else if (plugin_pos == 1)
            {
                return item_pos + QPoint(-1 * window_name_label->parentWidget()->width() - 25, mouse_pos.y() - window_name_label->parentWidget()->height() / 2);
            }
            else if (plugin_pos == 2)
            {
                return item_pos + QPoint(mouse_pos.x() - window_name_label->parentWidget()->width() / 2, -1 * window_name_label->parentWidget()->height() - 25);
            }
            else if (plugin_pos == 3)
            {
                return item_pos + QPoint(item_size.width() + 5, mouse_pos.y() - window_name_label->parentWidget()->height() / 2);
            }
        }
    }
    *isValid = false;
    return QPoint();
}

bool Window_Container::pluginSetShowCarrierCloseButton(PluginsItemInterface::Carrier_Type carrier_type, bool *isValid)
{
    if (carrier_type == PluginsItemInterface::Carrier_Type::Plugin_Tips || carrier_type == PluginsItemInterface::Carrier_Type::Plugin_Popup)
    {
        *isValid = true;
    }
    else
    {
        *isValid = false;
    }
    return false;
}

void Window_Container::init(PluginProxyInterface *proxyInter)
{
    m_proxyInter = proxyInter;
    if (!m_display)
    {
        m_display = XOpenDisplay(nullptr);
    }
    if (!pluginIsDisable())
    {
        pluginStateSwitched();
    }
    pluginStateSwitched();
    Public_Config::load();
}

QWidget *Window_Container::itemWidget(const QString &itemKey)
{
    (void) itemKey;
    return item_widget;
}

QWidget *Window_Container::itemTipsWidget(const QString &itemKey)
{
    (void) itemKey;
    QString text = tr("Window_Container");
    QPoint pos = item_widget->mapFromGlobal(QCursor::pos());
    for (int i = 0; i < item_list.count(); ++i)
    {
        if (item_list[i]->geometry().contains(pos))
        {
            text = item_list[i]->get_text();
            break;
        }
    }
    window_name_label->setText(text);
    window_name_label->setFont(Public_Config::tips_text_font);
    window_name_label->setStyleSheet(QString("QLabel{color: rgba(%1,%2,%3,%4)}").arg(Public_Config::tips_text_color.red()).arg(Public_Config::tips_text_color.green()).arg(Public_Config::tips_text_color.blue()).arg(Public_Config::tips_text_color.alpha()));
    return window_name_label;
}

QWidget *Window_Container::itemPopupApplet(const QString &itemKey)
{
    (void) itemKey;
    return nullptr;
}

void Window_Container::selectWindow()
{
    if (m_is_wayland)
    {
        QMessageBox::information(nullptr, tr("不支持"), tr("Wayland 环境下不支持选择窗口"));
        return;
    }
    if (m_picker)
    {
        return;
    }
    if (!m_display)
    {
        m_display = XOpenDisplay(nullptr);
    }
    if (!m_display)
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("X11 不可用"));
        return;
    }
    m_picker = new Window_Picker(m_display, this);
    QPointer<Window_Container> guard(this);
    m_picker->start([guard](bool ok, Window winId)
    {
        if (!guard)
        {
            return;
        }
        guard->m_picker = nullptr;
        if (!ok || winId == 0)
        {
            return;
        }
        guard->finish_select_window(winId);
    });
}

void Window_Container::finish_select_window(Window windowId)
{
    QPointer<Window_Container> guard(this);
    QTimer::singleShot(0, this, [guard, windowId]()
    {
        if (!guard)
        {
            return;
        }
        guard->do_finish_select_window(windowId);
    });
}

void Window_Container::do_finish_select_window(Window windowId)
{
    if (!m_display)
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("X11 不可用"));
        return;
    }
    pid_t pid = get_window_pid(m_display, windowId);
    QString windowType = get_window_class(m_display, windowId);

    if (pid == static_cast<pid_t>(QCoreApplication::applicationPid()))
    {
        QMessageBox::question(nullptr, tr("无法设置"), tr("(#`Д´)ﾉ不能自食其果!\nPID:") + QString("%1").arg(pid));
        return;
    }

    QMessageBox::StandardButton final_ans = QMessageBox::question(nullptr, tr("确认WinId"), tr("选取WinId:") + QString("%1").arg(windowId));
    if (final_ans != QMessageBox::StandardButton::Yes)
    {
        return;
    }

    if (windowType != "_NET_WM_WINDOW_TYPE_NORMAL")
    {
        QMessageBox::StandardButton final_ans2 = QMessageBox::question(nullptr, tr("确认_NET_WM_WINDOW_TYPE"), tr(">_<|||窗口类型:") + QString("%1").arg(windowType) + tr("\n强行设置可能出错,是否设置"));
        if (final_ans2 != QMessageBox::StandardButton::Yes)
        {
            return;
        }
    }

    WindowItem *window_item = new WindowItem(item_widget, windowId, &plugin_pos, m_is_wayland, m_display);
    item_list << window_item;
    item_widget->fresh_size();

    if (update_sender)
    {
        update_sender->Send();
    }
    else
    {
        pluginStateSwitched();
        pluginStateSwitched();
    }
}

void Window_Container::checkWindowStatus()
{
    if (m_is_wayland)
    {
        return;
    }
    for (int i = 0; i < item_list.count(); ++i)
    {
        item_list[i]->checkWindowStatus();
    }
    item_widget->fresh_size();
}

void Window_Container::Trans_Update()
{
    if (window_name_label)
    {
        window_name_label->setText(tr("Window_Container"));
    }
}

void Window_Container::update_style_V2(QColor m_theme_color,
                                       QColor m_theme_background_color,
                                       QColor m_theme_text_color,
                                       QColor m_select_text_color,
                                       QColor m_disabled_text_color,
                                       const QString &m_checked_icon_path,
                                       const QString &m_checked_and_selected_icon_path,
                                       int m_radius,
                                       int m_icon_size,
                                       QColor m_separator_color,
                                       int m_min_width,
                                       int m_on_blurring_type,
                                       QFont m_text_font,
                                       QColor m_menu_outline_color,
                                       int m_menu_outline_width,
                                       int m_blur_radius,
                                       QColor m_highlight_color)
{
    (void) m_theme_color;
    (void) m_theme_background_color;
    (void) m_select_text_color;
    (void) m_disabled_text_color;
    (void) m_checked_icon_path;
    (void) m_checked_and_selected_icon_path;
    (void) m_radius;
    (void) m_icon_size;
    (void) m_min_width;
    (void) m_on_blurring_type;
    (void) m_menu_outline_color;
    (void) m_menu_outline_width;
    (void) m_blur_radius;

    Public_Config::apply_host_theme(m_text_font, m_theme_text_color, m_highlight_color, m_separator_color);

    if (!m_is_wayland)
    {
        checkWindowStatus();
    }
    if (window_name_label)
    {
        window_name_label->setFont(Public_Config::tips_text_font);
        window_name_label->setStyleSheet(QString("QLabel{color: rgba(%1,%2,%3,%4)}").arg(Public_Config::tips_text_color.red()).arg(Public_Config::tips_text_color.green()).arg(Public_Config::tips_text_color.blue()).arg(Public_Config::tips_text_color.alpha()));
    }
}

void Window_Container::call_plugin_save()
{
    Public_Config::save();
}

QString Window_Container::get_window_class(Display *display, Window window)
{
    if (!display)
    {
        return QString();
    }
    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long bytes_after;
    unsigned char *prop = nullptr;
    QString result = "";
    Atom new_wm_TYPE = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
    if (XGetWindowProperty(display, window, new_wm_TYPE, 0, 1024, False, XA_ATOM, &actual_type, &actual_format, &nitems, &bytes_after, &prop) != Success)
    {
        return result;
    }
    if (prop && nitems > 0)
    {
        result = QString::fromStdString(XGetAtomName(display, reinterpret_cast<Atom *>(prop)[0]));
    }
    if (prop)
    {
        XFree(prop);
    }
    return result;
}

pid_t Window_Container::get_window_pid(Display *display, Window window)
{
    if (!display)
    {
        return 0;
    }
    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long bytes_after;
    unsigned char *prop = nullptr;
    pid_t pid = 0;
    Atom new_wm_pid = XInternAtom(display, "_NET_WM_PID", False);
    if (XGetWindowProperty(display, window, new_wm_pid, 0, 1, False, XA_CARDINAL, &actual_type, &actual_format, &nitems, &bytes_after, &prop) != Success)
    {
        return pid;
    }
    if (prop && nitems > 0)
    {
        pid = *reinterpret_cast<pid_t *>(prop);
    }
    if (prop)
    {
        XFree(prop);
    }
    return pid;
}

void Window_Container::pluginGetCarrierQMenu(Carrier_Type carrier_type, QMenu *menu, P_Sender * const action_sender)
{
    if (carrier_type == PluginsItemInterface::Carrier_Type::Plugin_Item)
    {
        item_widget->set_carrier_menu(menu, action_sender);
    }
}
