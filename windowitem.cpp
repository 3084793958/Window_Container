#include "windowitem.h"
#include "public_config.h"
#include <QDebug>
#include <QMessageBox>

WindowItem::WindowItem(QWidget *parent, Window m_targetWindow, int *m_plugin_pos_ptr, bool m_is_wayland, Display *m_display)
    : QWidget(parent)
    , targetWindow(m_targetWindow)
    , plugin_pos_ptr(m_plugin_pos_ptr)
    , is_wayland(m_is_wayland)
    , display(m_display)
{
    checkWindowStatus();
    show();
}

void WindowItem::show_wayland_unsupported_warning()
{
    static bool s_shown = false;
    if (s_shown)
    {
        return;
    }
    s_shown = true;
    QMessageBox::information(nullptr, tr("不支持"), tr("Wayland 环境下不支持窗口管理操作"));
}

void WindowItem::checkWindowStatus()
{
    if (is_wayland)
    {
        return;
    }
    m_titleLabel->setFont(Public_Config::text_font);
    m_titleLabel->setStyleSheet(QString("QLabel{color: rgba(%1,%2,%3,%4)}").arg(Public_Config::text_color.red()).arg(Public_Config::text_color.green()).arg(Public_Config::text_color.blue()).arg(Public_Config::text_color.alpha()));
    if (!isWindowActive())
    {
        m_statusLine->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4)").arg(Public_Config::status_color.red()).arg(Public_Config::status_color.green()).arg(Public_Config::status_color.blue()).arg(Public_Config::status_color.alpha()));
    }
    else
    {
        m_statusLine->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4)").arg(Public_Config::status_active_color.red()).arg(Public_Config::status_active_color.green()).arg(Public_Config::status_active_color.blue()).arg(Public_Config::status_active_color.alpha()));
    }
    set_short_border(Public_Config::container_height);
}

void WindowItem::set_short_border(int border)
{
    m_titleLabel->setText(fetchWindowTitle());
    m_iconLabel->setPixmap(fetchWindowIcon().pixmap(border - 4, border - 4));
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(border - 4);
    m_iconLabel->resize(border - 4, border - 4);
    QRect text_rect = QFontMetrics(m_titleLabel->font()).boundingRect(m_titleLabel->text());
    if (*plugin_pos_ptr == 0)
    {
        m_iconLabel->move(0, 2);
        m_titleLabel->move(border - 2, 2);
        m_titleLabel->resize(text_rect.width(), border - 4);
        m_statusLine->setFixedSize(m_titleLabel->x() + m_titleLabel->width(), 2);
        m_statusLine->resize(m_titleLabel->x() + m_titleLabel->width(), 2);
        m_statusLine->move(0, 0);
        QWidget::resize(m_titleLabel->x() + m_titleLabel->width(), border);
    }
    else if (*plugin_pos_ptr == 1)
    {
        m_iconLabel->move(1, 0);
        m_titleLabel->move(1, border - 2);
        m_titleLabel->resize(border - 4, text_rect.width());
        m_statusLine->setFixedSize(3, m_titleLabel->y() + m_titleLabel->height());
        m_statusLine->resize(3, m_titleLabel->y() + m_titleLabel->height());
        m_statusLine->move(border - 2, 0);
        QWidget::resize(border, m_titleLabel->y() + m_titleLabel->height());
    }
    else if (*plugin_pos_ptr == 2)
    {
        m_iconLabel->move(0, 2);
        m_titleLabel->move(border - 2, 2);
        m_titleLabel->resize(text_rect.width(), border - 4);
        m_statusLine->setFixedSize(m_titleLabel->x() + m_titleLabel->width(), 3);
        m_statusLine->resize(m_titleLabel->x() + m_titleLabel->width(), 3);
        m_statusLine->move(0, border - 2);
        QWidget::resize(m_titleLabel->x() + m_titleLabel->width(), border);
    }
    else if (*plugin_pos_ptr == 3)
    {
        m_iconLabel->move(2, 0);
        m_titleLabel->move(1, border - 2);
        m_titleLabel->resize(border - 4, text_rect.width());
        m_statusLine->setFixedSize(2, m_titleLabel->y() + m_titleLabel->height());
        m_statusLine->resize(2, m_titleLabel->y() + m_titleLabel->height());
        m_statusLine->move(0, 0);
        QWidget::resize(border, m_titleLabel->y() + m_titleLabel->height());
    }
    if (item_size.isValid() && item_size != QSize(0, 0))
    {
        QWidget::resize(item_size);
    }
}

static Atom net_wm_state(Display *dpy)
{
    return XInternAtom(dpy, "_NET_WM_STATE", False);
}

static Atom net_wm_state_maximized_vert(Display *dpy)
{
    return XInternAtom(dpy, "_NET_WM_STATE_MAXIMIZED_VERT", False);
}

static Atom net_wm_state_maximized_horz(Display *dpy)
{
    return XInternAtom(dpy, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
}

static Atom net_wm_state_hidden(Display *dpy)
{
    return XInternAtom(dpy, "_NET_WM_STATE_HIDDEN", False);
}

static Atom wm_delete_window(Display *dpy)
{
    return XInternAtom(dpy, "WM_DELETE_WINDOW", False);
}

static Atom wm_protocols(Display *dpy)
{
    return XInternAtom(dpy, "WM_PROTOCOLS", False);
}

static void send_net_wm_state(Display *dpy, Window win, long action, Atom prop1, Atom prop2 = 0)
{
    if (!dpy)
    {
        return;
    }
    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.window = win;
    xev.xclient.message_type = net_wm_state(dpy);
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = action;
    xev.xclient.data.l[1] = static_cast<long>(prop1);
    xev.xclient.data.l[2] = static_cast<long>(prop2);
    xev.xclient.data.l[3] = 1;
    xev.xclient.data.l[4] = 0;
    XSendEvent(dpy, DefaultRootWindow(dpy), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    XFlush(dpy);
}

void WindowItem::show_window_action()
{
    if (is_wayland || !display)
    {
        show_wayland_unsupported_warning();
        return;
    }
    send_net_wm_state(display, targetWindow, 0, net_wm_state_hidden(display));
    XWindowAttributes attrs;
    if (XGetWindowAttributes(display, targetWindow, &attrs))
    {
        if (attrs.map_state == IsUnmapped)
        {
            XMapWindow(display, targetWindow);
        }
    }
    XFlush(display);
    active_window();
}

void WindowItem::hide_window_action()
{
    if (is_wayland || !display)
    {
        show_wayland_unsupported_warning();
        return;
    }
    send_net_wm_state(display, targetWindow, 1, net_wm_state_hidden(display));
    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.window = targetWindow;
    xev.xclient.message_type = XInternAtom(display, "WM_CHANGE_STATE", False);
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 3;
    XSendEvent(display, DefaultRootWindow(display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    XFlush(display);
}

void WindowItem::max_window_size_action()
{
    if (is_wayland || !display)
    {
        show_wayland_unsupported_warning();
        return;
    }
    send_net_wm_state(display, targetWindow, 1, net_wm_state_maximized_vert(display), net_wm_state_maximized_horz(display));
    active_window();
}

void WindowItem::normal_window_size_action()
{
    if (is_wayland || !display)
    {
        show_wayland_unsupported_warning();
        return;
    }
    send_net_wm_state(display, targetWindow, 0, net_wm_state_maximized_vert(display), net_wm_state_maximized_horz(display));
    active_window();
}

void WindowItem::close_window_action()
{
    if (is_wayland || !display)
    {
        show_wayland_unsupported_warning();
        return;
    }
    Atom wmDelete = wm_delete_window(display);
    Atom protocols = wm_protocols(display);
    Atom *supported = nullptr;
    int count = 0;
    if (XGetWMProtocols(display, targetWindow, &supported, &count))
    {
        for (int i = 0; i < count; ++i)
        {
            if (supported[i] == wmDelete)
            {
                XEvent xev;
                xev.xclient.type = ClientMessage;
                xev.xclient.window = targetWindow;
                xev.xclient.message_type = protocols;
                xev.xclient.format = 32;
                xev.xclient.data.l[0] = static_cast<long>(wmDelete);
                xev.xclient.data.l[1] = CurrentTime;
                XSendEvent(display, targetWindow, False, 0, &xev);
                XFlush(display);
                XFree(supported);
                return;
            }
        }
        XFree(supported);
    }
    XKillClient(display, targetWindow);
    XFlush(display);
}

void WindowItem::active_window()
{
    if (is_wayland || !display)
    {
        show_wayland_unsupported_warning();
        return;
    }
    Atom net_active_window = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.window = targetWindow;
    xev.xclient.message_type = net_active_window;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 1;
    xev.xclient.data.l[1] = CurrentTime;
    xev.xclient.data.l[2] = 0;
    XSendEvent(display, DefaultRootWindow(display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    XFlush(display);
}

void WindowItem::click_window_action()
{
    if (is_wayland || !display)
    {
        show_wayland_unsupported_warning();
        return;
    }
    if (is_window_hidden())
    {
        active_window();
        XRaiseWindow(display, targetWindow);
        XFlush(display);
    }
    else
    {
        hide_window_action();
    }
}

void WindowItem::set_size_action()
{
    bool ok = false;
    int new_width = QInputDialog::getInt(nullptr, tr("获取数值"), tr("输入width:\n若width和height都为0,则会以默认大小显示"), item_size.width(), 0, 2147483647, 1, &ok);
    if (!ok)
    {
        return;
    }
    ok = false;
    int new_height = QInputDialog::getInt(nullptr, tr("获取数值"), tr("输入height:\n若width和height都为0,则会以默认大小显示"), item_size.height(), 0, 2147483647, 1, &ok);
    if (!ok)
    {
        return;
    }
    item_size = QSize(new_width, new_height);
}

bool WindowItem::is_window_hidden()
{
    if (is_wayland || !display)
    {
        return false;
    }
    bool is_hidden = false;
    Atom net_wm_state_atom = XInternAtom(display, "_NET_WM_STATE", False);
    Atom hidden_atom = net_wm_state_hidden(display);
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char *prop_data = nullptr;
    if (XGetWindowProperty(display, targetWindow, net_wm_state_atom, 0, 1024, False, XA_ATOM, &actual_type, &actual_format, &nitems, &bytes_after, &prop_data) == Success)
    {
        if (actual_type == XA_ATOM && prop_data)
        {
            Atom *atoms = reinterpret_cast<Atom *>(prop_data);
            for (unsigned long i = 0; i < nitems; ++i)
            {
                if (atoms[i] == hidden_atom)
                {
                    is_hidden = true;
                    break;
                }
            }
        }
        if (prop_data)
        {
            XFree(prop_data);
        }
    }
    return is_hidden;
}

QString WindowItem::get_text()
{
    QString text = m_titleLabel->text();
    text.chop(1);
    return text;
}

QString WindowItem::fetchWindowTitle()
{
    if (is_wayland || !display)
    {
        return tr("未知窗口");
    }
    QString title;
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char *prop = nullptr;
    // 1. 尝试 _NET_WM_NAME (UTF8_STRING)
    Atom net_wm_name = XInternAtom(display, "_NET_WM_NAME", False);
    if (XGetWindowProperty(display, targetWindow, net_wm_name, 0, 1024, False, AnyPropertyType, &actual_type, &actual_format, &nitems, &bytes_after, &prop) == Success)
    {
        if (prop && nitems > 0 && actual_format == 8)
        {
            title = QString::fromUtf8(reinterpret_cast<char *>(prop), static_cast<int>(nitems));
        }
        if (prop)
        {
            XFree(prop);
        }
    }
    if (!title.isEmpty())
    {
        return title;
    }
    // 2. 尝试 WM_NAME
    Atom wm_name = XInternAtom(display, "WM_NAME", False);
    prop = nullptr;
    if (XGetWindowProperty(display, targetWindow, wm_name, 0, 1024, False, AnyPropertyType, &actual_type, &actual_format, &nitems, &bytes_after, &prop) == Success)
    {
        if (prop && nitems > 0 && actual_format == 8)
        {
            title = QString::fromUtf8(reinterpret_cast<char *>(prop), static_cast<int>(nitems));
        }
        if (prop)
        {
            XFree(prop);
        }
    }
    return title.isEmpty() ? tr("未知窗口") : title;
}

bool WindowItem::isWindowActive()
{
    if (is_wayland || !display)
    {
        return false;
    }
    bool active = false;
    Atom net_active_window = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char *prop = nullptr;
    if (XGetWindowProperty(display, DefaultRootWindow(display), net_active_window, 0, 1, False, XA_WINDOW, &actual_type, &actual_format, &nitems, &bytes_after, &prop) == Success)
    {
        if (prop && nitems > 0 && actual_format == 32)
        {
            unsigned long active_win = *reinterpret_cast<unsigned long *>(prop);
            active = (active_win == targetWindow);
        }
        if (prop)
        {
            XFree(prop);
        }
    }
    return active;
}

bool WindowItem::isWindowExists()
{
    if (is_wayland || !display)
    {
        return false;
    }
    XWindowAttributes attrs;
    return XGetWindowAttributes(display, targetWindow, &attrs) != 0;
}

QIcon WindowItem::fetchWindowIcon()
{
    if (is_wayland || !display)
    {
        return QApplication::style()->standardIcon(QStyle::SP_ComputerIcon);
    }
    Atom net_wm_icon = XInternAtom(display, "_NET_WM_ICON", False);
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char *data = nullptr;
    if (XGetWindowProperty(display, targetWindow, net_wm_icon, 0, LONG_MAX, False, XA_CARDINAL, &actual_type, &actual_format, &nitems, &bytes_after, &data) != Success)
    {
        return QApplication::style()->standardIcon(QStyle::SP_ComputerIcon);
    }
    QIcon icon;
    if (data && actual_type == XA_CARDINAL && actual_format == 32)
    {
        unsigned long *icon_data = reinterpret_cast<unsigned long *>(data);
        unsigned long idx = 0;
        while (idx + 2 <= nitems)
        {
            unsigned long width = icon_data[idx++];
            unsigned long height = icon_data[idx++];
            if (width == 0 || height == 0)
            {
                break;
            }
            unsigned long pixel_count = width * height;
            if (idx + pixel_count > nitems)
            {
                break;
            }
            QImage img(static_cast<int>(width), static_cast<int>(height), QImage::Format_ARGB32);
            for (unsigned long y = 0; y < height; ++y)
            {
                QRgb *scanLine = reinterpret_cast<QRgb *>(img.scanLine(static_cast<int>(y)));
                for (unsigned long x = 0; x < width; ++x)
                {
                    unsigned long pixel = icon_data[idx++];
                    scanLine[x] = static_cast<QRgb>(pixel);
                }
            }
            icon.addPixmap(QPixmap::fromImage(img));
        }
    }
    if (data)
    {
        XFree(data);
    }
    if (!icon.isNull())
    {
        return icon;
    }
    return QApplication::style()->standardIcon(QStyle::SP_ComputerIcon);
}

Towards_Label::Towards_Label(QWidget *parent, int **m_plugin_pos_ptr)
    : QLabel(parent)
    , plugin_pos_ptr(m_plugin_pos_ptr)
{}

void Towards_Label::paintEvent(QPaintEvent *event)
{
    if (**plugin_pos_ptr == 0 || **plugin_pos_ptr == 2)
    {
        QLabel::paintEvent(event);
    }
    else
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);
        painter.save();
        painter.translate(width() / 2, height() / 2);
        painter.rotate(-90);
        QFontMetrics fm = painter.fontMetrics();
        int textWidth = fm.horizontalAdvance(text());
        int textHeight = fm.height();
        painter.drawText(-textWidth / 2, textHeight / 2, text());
        painter.restore();
    }
}
