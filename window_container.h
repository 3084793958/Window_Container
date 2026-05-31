#ifndef WINDOW_CONTAINER_H
#define WINDOW_CONTAINER_H

#include "interfaces/dde-dock/v1.2.3/pluginsiteminterface.h"

#include <QLabel>

#include "plugin_item_widget.h"

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
private slots:
    void selectWindow();
    void checkWindowStatus();
private:
    P_Sender * update_sender = nullptr;
private:
    int plugin_pos = 0;
private:
    QProcess *m_xwininfoProcess = nullptr;
    QTimer *m_timer = new QTimer(this);
    QList<WindowItem *> item_list;
    QLabel *window_name_label = new QLabel(tr("Window_Container"));
    Plugin_Item_Widget *item_widget = new Plugin_Item_Widget(&item_list, &plugin_pos);
private:
    Display *display = nullptr;
    static QString get_window_class(Display *display, Window window);
    static pid_t get_window_pid(Display *display, Window window);
};

#endif // WINDOW_CONTAINER_H
