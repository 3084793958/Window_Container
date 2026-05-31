#ifndef WINDOWITEM_H
#define WINDOWITEM_H

#include <QtWidgets>

#include <X11/Xlib.h>
#include <QX11Info>
#include <X11/Xatom.h>
#undef Status
#undef Bool
#undef None
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef CursorShape

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
    explicit WindowItem(QWidget *parent, Window m_targetWindow, int *m_plugin_pos_ptr);
    void checkWindowStatus();
    void set_short_border(int border);
    void show_window_action();
    void hide_window_action();
    void max_window_size_action();
    void normal_window_size_action();
    void close_window_action();
    void click_window_action();
    void active_window();
    bool is_window_hidden();
    QString get_text();
private:
    Window targetWindow = 0;
    int *plugin_pos_ptr = nullptr;
private:
    QLabel *m_iconLabel = new QLabel(this);
    Towards_Label *m_titleLabel = new Towards_Label(this, &plugin_pos_ptr);
    QFrame *m_statusLine = new QFrame(this);
private:
    QString fetchWindowTitle();
    bool isWindowActive();
    QIcon fetchWindowIcon();
public:
    bool isWindowExists();
};

#endif // WINDOWITEM_H
