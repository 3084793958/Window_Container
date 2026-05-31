#include "windowitem.h"
#include "public_config.h"
#include <QDebug>
WindowItem::WindowItem(QWidget *parent, Window m_targetWindow, int *m_plugin_pos_ptr)
    :QWidget(parent)
    ,targetWindow(m_targetWindow)
    ,plugin_pos_ptr(m_plugin_pos_ptr)
{
    checkWindowStatus();

    show();
}
void WindowItem::checkWindowStatus()
{
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

    if (*plugin_pos_ptr == 0)//Top
    {
        m_iconLabel->move(0, 2);
        m_titleLabel->move(border - 2, 2);
        m_titleLabel->resize(text_rect.width(), border - 4);
        m_statusLine->setFixedSize(m_titleLabel->x() + m_titleLabel->width(), 2);
        m_statusLine->resize(m_titleLabel->x() + m_titleLabel->width(), 2);
        m_statusLine->move(0, 0);
        QWidget::resize(m_titleLabel->x() + m_titleLabel->width(), border);
    }
    else if (*plugin_pos_ptr == 1)//Right
    {
        m_iconLabel->move(1, 0);
        m_titleLabel->move(1, border - 2);//特殊
        m_titleLabel->resize(border - 4, text_rect.width());
        m_statusLine->setFixedSize(3, m_titleLabel->y() + m_titleLabel->height());
        m_statusLine->resize(3, m_titleLabel->y() + m_titleLabel->height());
        m_statusLine->move(border - 2, 0);
        QWidget::resize(border, m_titleLabel->y() + m_titleLabel->height());
    }
    else if (*plugin_pos_ptr == 2)//Bottom
    {
        m_iconLabel->move(0, 2);
        m_titleLabel->move(border - 2, 2);
        m_titleLabel->resize(text_rect.width(), border - 4);
        m_statusLine->setFixedSize(m_titleLabel->x() + m_titleLabel->width(), 3);
        m_statusLine->resize(m_titleLabel->x() + m_titleLabel->width(), 3);
        m_statusLine->move(0, border - 2);
        QWidget::resize(m_titleLabel->x() + m_titleLabel->width(), border);
    }
    else if (*plugin_pos_ptr == 3)//Left
    {
        m_iconLabel->move(2, 0);
        m_titleLabel->move(1, border - 2);
        m_titleLabel->resize(border - 4, text_rect.width());
        m_statusLine->setFixedSize(2, m_titleLabel->y() + m_titleLabel->height());
        m_statusLine->resize(2, m_titleLabel->y() + m_titleLabel->height());
        m_statusLine->move(0, 0);
        QWidget::resize(border, m_titleLabel->y() + m_titleLabel->height());
    }
}
static Atom net_wm_state()
{
    static Atom atom = XInternAtom(QX11Info::display(), "_NET_WM_STATE", False);
    return atom;
}
static Atom net_wm_state_maximized_vert()
{
    static Atom atom = XInternAtom(QX11Info::display(), "_NET_WM_STATE_MAXIMIZED_VERT", False);
    return atom;
}
static Atom net_wm_state_maximized_horz()
{
    static Atom atom = XInternAtom(QX11Info::display(), "_NET_WM_STATE_MAXIMIZED_HORZ", False);
    return atom;
}
static Atom net_wm_state_hidden()
{
    static Atom atom = XInternAtom(QX11Info::display(), "_NET_WM_STATE_HIDDEN", False);
    return atom;
}
static Atom wm_delete_window()
{
    static Atom atom = XInternAtom(QX11Info::display(), "WM_DELETE_WINDOW", False);
    return atom;
}
static Atom wm_protocols()
{
    static Atom atom = XInternAtom(QX11Info::display(), "WM_PROTOCOLS", False);
    return atom;
}
static void send_net_wm_state(Window win, long action, Atom prop1, Atom prop2 = 0)
{
    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.window = win;
    xev.xclient.message_type = net_wm_state();
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = action;
    xev.xclient.data.l[1] = static_cast<long>(prop1);
    xev.xclient.data.l[2] = static_cast<long>(prop2);
    xev.xclient.data.l[3] = 1;
    xev.xclient.data.l[4] = 0;
    XSendEvent(QX11Info::display(), DefaultRootWindow(QX11Info::display()), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    XFlush(QX11Info::display());
}
void WindowItem::show_window_action()
{
    send_net_wm_state(targetWindow, 0, net_wm_state_hidden());
    XWindowAttributes attrs;
    if (XGetWindowAttributes(QX11Info::display(), targetWindow, &attrs))
    {
        if (attrs.map_state == IsUnmapped)
        {
            XMapWindow(QX11Info::display(), targetWindow);
        }
    }
    XFlush(QX11Info::display());
    active_window();
}
void WindowItem::hide_window_action()
{
    send_net_wm_state(targetWindow, 1, net_wm_state_hidden());
    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.window = targetWindow;
    xev.xclient.message_type = XInternAtom(QX11Info::display(), "WM_CHANGE_STATE", False);
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 3;
    XSendEvent(QX11Info::display(), DefaultRootWindow(QX11Info::display()), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    XFlush(QX11Info::display());
}
void WindowItem::max_window_size_action()
{
    send_net_wm_state(targetWindow, 1, net_wm_state_maximized_vert(), net_wm_state_maximized_horz());
    active_window();
}
void WindowItem::normal_window_size_action()
{
    send_net_wm_state(targetWindow, 0, net_wm_state_maximized_vert(), net_wm_state_maximized_horz());
    active_window();
}
void WindowItem::close_window_action()
{
    Atom wmDelete = wm_delete_window();
    Atom protocols = wm_protocols();
    Atom *supported = nullptr;
    int count = 0;
    if (XGetWMProtocols(QX11Info::display(), targetWindow, &supported, &count))
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
                XSendEvent(QX11Info::display(), targetWindow, False, 0, &xev);
                XFlush(QX11Info::display());
                XFree(supported);
                return;
            }
        }
        XFree(supported);
    }
    XKillClient(QX11Info::display(), targetWindow);
    XFlush(QX11Info::display());
}
void WindowItem::active_window()
{
    Atom net_active_window = XInternAtom(QX11Info::display(), "_NET_ACTIVE_WINDOW", False);
    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.window = targetWindow;
    xev.xclient.message_type = net_active_window;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 1;
    xev.xclient.data.l[1] = CurrentTime;
    xev.xclient.data.l[2] = 0;
    XSendEvent(QX11Info::display(), DefaultRootWindow(QX11Info::display()), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    XFlush(QX11Info::display());
}
void WindowItem::click_window_action()
{
    if (is_window_hidden())
    {
        active_window();
        XRaiseWindow(QX11Info::display(), targetWindow);
        XFlush(QX11Info::display());
    }
    else
    {
        hide_window_action();
    }
}
bool WindowItem::is_window_hidden()
{
    bool is_hidden = false;
    Atom net_wm_state = XInternAtom(QX11Info::display(), "_NET_WM_STATE", False);
    Atom hidden_atom = net_wm_state_hidden();
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char *prop_data = nullptr;
    if (XGetWindowProperty(QX11Info::display(), targetWindow, net_wm_state, 0, 1024, False, XA_ATOM, &actual_type, &actual_format, &nitems, &bytes_after, &prop_data) == Success)
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
        if (prop_data) XFree(prop_data);
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
    QProcess proc;

    proc.start("xdotool", QStringList() << "getwindowname" << QString::number(targetWindow));
    proc.waitForFinished(1000);
    QString title = proc.readAllStandardOutput();
    if (!title.isEmpty())
    {
        return title;
    }
    proc.start("xprop", QStringList() << "-id" << QString::number(targetWindow) << "WM_NAME");
    proc.waitForFinished(1000);
    QString output = proc.readAllStandardOutput();
    QRegularExpression re("WM_NAME\\(STRING\\) = \"(.*)\"");
    QRegularExpressionMatch match = re.match(output);
    if (match.hasMatch())
    {
        return match.captured(1);
    }

    return tr("未知窗口");
}
bool WindowItem::isWindowActive()
{
    QProcess proc;
    proc.start("xdotool", QStringList() << "getactivewindow");
    proc.waitForFinished(1000);
    quint32 activeId = proc.readAllStandardOutput().toUInt();
    return activeId == targetWindow;
}
bool WindowItem::isWindowExists()
{
    QProcess proc;
    proc.start("xwininfo", {"-id", QString::number(targetWindow)});
    proc.waitForFinished(1000);
    return proc.exitCode() == 0;
}
QIcon WindowItem::fetchWindowIcon()
{
    if (!QX11Info::display())
    {
        return QApplication::style()->standardIcon(QStyle::SP_ComputerIcon);
    }
    Atom net_wm_icon = XInternAtom(QX11Info::display(), "_NET_WM_ICON", False);
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char *data = nullptr;

    if (XGetWindowProperty(QX11Info::display(), targetWindow, net_wm_icon, 0, LONG_MAX, False, XA_CARDINAL, &actual_type, &actual_format, &nitems, &bytes_after, &data) != Success)
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
                QRgb *scanLine = reinterpret_cast<QRgb*>(img.scanLine(static_cast<int>(y)));
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
    :QLabel(parent)
    ,plugin_pos_ptr(m_plugin_pos_ptr)
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
