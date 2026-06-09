#ifndef PLUGIN_ITEM_WIDGET_H
#define PLUGIN_ITEM_WIDGET_H

#include <QtWidgets>
#include <interfaces/easy_desktop_class.h>

#include "windowitem.h"

class Plugin_Item_Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Plugin_Item_Widget(QList<WindowItem *> *m_windowitem_list_ptr, int *m_pos_ptr, QWidget *parent = nullptr);
    void set_carrier_menu(QMenu *carrier_menu, P_Sender * const m_menu_sender);
    void fresh_size();
    virtual QSize sizeHint() const;
signals:
    void Add_Window();
    void remove_window();
private:
    P_Sender *menu_sender = nullptr;
    QAction *item_carrier_menu = nullptr;
    QMenu *menu = new QMenu(this);
    QAction *show_window_action = new QAction(tr("显示窗口"), this);
    QAction *hide_window_action = new QAction(tr("最小化"), this);
    QAction *max_window_size_action = new QAction(tr("最大化"), this);
    QAction *normal_window_size_action = new QAction(tr("正常化"), this);
    QAction *close_window_action = new QAction(tr("关闭窗口"), this);
    QAction *add_window_action = new QAction(tr("添加窗口"), this);
    QAction *remove_this_window_action = new QAction(tr("移除该窗口"), this);
    QAction *set_window_index_action = new QAction(tr("修改窗口索引"), this);

    QAction *refresh_size_action = new QAction(tr("刷新插件大小"), this);
    QAction *set_size_action = new QAction(tr("设置item大小"), this);

    QMenu *style_set_menu = new QMenu(tr("外观设置"), this);
    QAction *set_text_font_action = new QAction(tr("设置字体"), this);
    QAction *set_text_color_action = new QAction(tr("设置文字颜色"), this);
    QAction *set_status_color_active_action = new QAction(tr("设置状态栏颜色(活动)"), this);
    QAction *set_status_color_action = new QAction(tr("设置状态栏颜色(非活动)"), this);
    QAction *set_container_height_action = new QAction(tr("设置容器高度"), this);
    QAction *set_tips_text_font_action = new QAction(tr("设置tips字体"), this);
    QAction *set_tips_text_color_action = new QAction(tr("设置tips文字颜色"), this);

    QAction *save_action = new QAction(tr("储存"), this);
    QAction *load_action = new QAction(tr("读取"), this);
private:
    QList<WindowItem *> *windowitem_list_ptr = nullptr;
    int *pos_ptr = nullptr;
    QPoint mouse_move_pos = QPoint();
private:
    void contextMenuEvent(QContextMenuEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // PLUGIN_ITEM_WIDGET_H
