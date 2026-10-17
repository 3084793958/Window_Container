#ifndef WINDOWITEM_H
#define WINDOWITEM_H

// 顺序：Qt -> Xlib -> undef
#include <QtWidgets>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#undef Bool
#undef Status
#undef CursorShape
#undef None
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef Expose
#undef FontChange
#undef Unsorted
#undef Above
#undef Below
#undef Complex
#undef Convex
#undef Nonconvex

class Towards_Label : public QLabel
{
    Q_OBJECT
public:
    explicit Towards_Label(QWidget *parent, int **m_plugin_pos_ptr);
    int **plugin_pos_ptr = nullptr;
protected:
    void paintEvent(QPaintEvent *event) override;
};

class WindowItem : public QWidget
{
    Q_OBJECT
public:
    explicit WindowItem(QWidget *parent, Window m_targetWindow, int *m_plugin_pos_ptr, bool m_is_wayland = false, Display *m_display = nullptr);
    void checkWindowStatus();
    void set_short_border(int border);
    void show_window_action();
    void hide_window_action();
    void max_window_size_action();
    void normal_window_size_action();
    void close_window_action();
    void click_window_action();
    void set_size_action();
    void active_window();
    bool is_window_hidden();
    QString get_text();
private:
    Window targetWindow = 0;
    int *plugin_pos_ptr = nullptr;
    QSize item_size = QSize(0, 0);
    bool is_wayland = false;
    Display *display = nullptr;
    QLabel *m_iconLabel = new QLabel(this);
    Towards_Label *m_titleLabel = new Towards_Label(this, &plugin_pos_ptr);
    QFrame *m_statusLine = new QFrame(this);
private:
    QString fetchWindowTitle();
    bool isWindowActive();
    QIcon fetchWindowIcon();
    void show_wayland_unsupported_warning();
public:
    bool isWindowExists();
};

#endif // WINDOWITEM_H
