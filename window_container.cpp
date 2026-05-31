#include "window_container.h"
#include <QMessageBox>
#include "public_config.h"
Window_Container::Window_Container(QObject *parent)
    :QObject(parent)
{
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
    if (m_proxyInter) m_proxyInter->saveValue(this, "disabled", disabledNew);
    if (disabledNew)
    {
        if (m_proxyInter) m_proxyInter->itemRemoved(this, pluginName());
    }
    else
    {
        if (m_proxyInter) m_proxyInter->itemAdded(this, pluginName());
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
    if (m_xwininfoProcess)
    {
        m_xwininfoProcess->kill();
        m_xwininfoProcess->deleteLater();
        m_xwininfoProcess = nullptr;
    }
    m_timer->stop();
    disconnect();
    window_name_label->deleteLater();
    item_widget->deleteLater();
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
    if (!pluginIsDisable())
    {
        pluginStateSwitched();
    }
    pluginStateSwitched();//为dde-dock和dde-top-panel而做,但什么都分辨不出来,只能一起做(Easy_Desktop:我也要执行吗)
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
    if (m_xwininfoProcess && m_xwininfoProcess->state() == QProcess::Running)
    {
        return;
    }
    m_xwininfoProcess = new QProcess;
    m_xwininfoProcess->start("/bin/bash", QStringList() << "-c" << "xwininfo -int | grep 'xwininfo: Window id:'");
    connect(m_xwininfoProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, [=]
    {
        QByteArray output = m_xwininfoProcess->readAllStandardOutput();
        QStringList windowIds = QString(output).split(" ", Qt::SkipEmptyParts);
        disconnect(m_xwininfoProcess, nullptr, this, nullptr);
        delete m_xwininfoProcess;
        m_xwininfoProcess = nullptr;
        if (windowIds.size() < 4)
        {
            return;
        }
        bool ok = false;
        Window windowId = windowIds[3].toULong(&ok);
        if (!ok)
        {
            return;
        }
        display = QX11Info::display();
        Window windowPId = static_cast<unsigned long>(get_window_pid(display, windowId));
        QString windowType = get_window_class(display, windowId);
        if (windowPId == static_cast<unsigned long>(QCoreApplication::applicationPid()))
        {
            QMessageBox::question(nullptr, tr("无法设置"),tr("(#`Д´)ﾉ不能自食其果!\nPID:") + QString("%1").arg(windowPId));
            return;
        }
        QMessageBox::StandardButton final_ans = QMessageBox::question(nullptr, tr("确认WinId"), tr("选取WinId:") + QString("%1").arg(windowId));
        if (final_ans != QMessageBox::StandardButton::Yes)
        {
            return;
        }
        if (windowType != "_NET_WM_WINDOW_TYPE_NORMAL")
        {
            QMessageBox::StandardButton final_ans = QMessageBox::question(nullptr, tr("确认_NET_WM_WINDOW_TYPE"), tr(">_<|||窗口类型:") + QString("%1").arg(windowType) + tr("\n强行设置可能出错,是否设置"));
            if (final_ans != QMessageBox::StandardButton::Yes)
            {
                return;
            }
        }
        WindowItem *window_item = new WindowItem(item_widget, windowId, &plugin_pos);
        item_list << window_item;
        item_widget->fresh_size();
        if (update_sender)
        {
            update_sender->Send();
        }
        else
        {
            pluginStateSwitched();
            pluginStateSwitched();//专为dde-top-panel准备的,但是由于dde-top-panel与dde-dock使用同一套解析器,想不到办法分辨,所以这也会作用于dde-dock上
        }
    });
}
void Window_Container::checkWindowStatus()
{
    for (int i = 0; i < item_list.count(); ++i)
    {
        item_list[i]->checkWindowStatus();
    }
    item_widget->fresh_size();
}
QString Window_Container::get_window_class(Display *display, Window window)
{
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
