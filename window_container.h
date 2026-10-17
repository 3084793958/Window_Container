#ifndef WINDOW_CONTAINER_H
#define WINDOW_CONTAINER_H

// 顺序：Qt 基础 -> Xlib -> undef（清理冲突宏）-> 内部头
#include "interfaces/dde-dock/v1.2.3/pluginsiteminterface.h"
#include <QLabel>
#include <QColor>
#include <QFont>
#include <QStringList>
#include <QPointer>
#include <QSocketNotifier>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>

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

#include "plugin_item_widget.h"

class Window_Picker : public QObject
{
    Q_OBJECT
public:
    explicit Window_Picker(Display *dpy, QObject *parent = nullptr);
    ~Window_Picker();
    void start(std::function<void(bool, Window)> callback);
private:
    void finish(bool success, Window win);
    static Window tryFindClientWindow(Display *dpy, Window win, Atom wmStateAtom);
    static Window getClientWindow(Display *dpy, Window win);
    Display *m_display;
    std::function<void(bool, Window)> m_callback;
    QSocketNotifier *m_notifier = nullptr;
    Cursor m_cursor = 0;
};

class Window_Container : public QObject, public PluginsItemInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginsItemInterface)
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface" FILE "Window_Container.json")
public:
    explicit Window_Container(QObject *parent = nullptr);
    virtual ~Window_Container() override;
    virtual const QString pluginName() const override;
    virtual const QString pluginDisplayName() const override;
    virtual bool pluginIsAllowDisable() override;
    virtual bool pluginIsDisable() override;
    virtual void pluginStateSwitched() override;
    virtual void init(PluginProxyInterface *proxyInter) override;
    virtual QWidget *itemWidget(const QString &itemKey) override;
    virtual QWidget *itemTipsWidget(const QString &itemKey) override;
    virtual QWidget *itemPopupApplet(const QString &itemKey) override;
    virtual bool pluginIsAllowUnload() override;
    virtual void pluginUnload() override;
    virtual void positionChanged(const Dock::Position position) override;
    bool pluginSetShowCarrierCloseButton(Carrier_Type carrier_type, bool *isValid) override;
    void pluginGetCarrierQMenu(Carrier_Type carrier_type, QMenu *menu, P_Sender * const action_sender) override;
    QSize pluginSetWidgetSize(PluginsItemInterface::Carrier_Type carrier_type, bool *isValid) override;
    virtual void pluginGetSender(P_Sender * const update_sender, P_Sender * const send_data_sender) override;
    virtual QPoint pluginSetCarrierPos(Carrier_Type carrier_type, QPoint item_pos, QSize item_size, bool *isValid) override;
    void Trans_Update() override;
    void update_style_V2(QColor m_theme_color,
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
                         QColor m_highlight_color) override;
    void call_plugin_save() override;
private slots:
    void selectWindow();
    void checkWindowStatus();
private:
    P_Sender *update_sender = nullptr;
    int plugin_pos = 0;
    bool m_is_wayland = false;
    QTimer *m_timer = new QTimer(this);
    QList<WindowItem *> item_list;
    QLabel *window_name_label = new QLabel(tr("Window_Container"));
    Plugin_Item_Widget *item_widget = new Plugin_Item_Widget(&item_list, &plugin_pos);
    Display *m_display = nullptr;
    Window_Picker *m_picker = nullptr;
    static QString get_window_class(Display *display, Window window);
    static pid_t get_window_pid(Display *display, Window window);
    void finish_select_window(Window windowId);
    void do_finish_select_window(Window windowId);
};

#endif // WINDOW_CONTAINER_H
