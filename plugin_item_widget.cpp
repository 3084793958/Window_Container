#include "plugin_item_widget.h"
#include "public_config.h"
Plugin_Item_Widget::Plugin_Item_Widget(QList<WindowItem *> *m_windowitem_list_ptr, int *m_pos_ptr, QWidget *parent)
    :QWidget(parent)
    ,windowitem_list_ptr(m_windowitem_list_ptr)
    ,pos_ptr(m_pos_ptr)
{
    menu->addAction(show_window_action);
    menu->addAction(max_window_size_action);
    menu->addAction(hide_window_action);
    menu->addAction(normal_window_size_action);
    menu->addAction(close_window_action);
    menu->addSeparator();
    menu->addAction(add_window_action);
    menu->addAction(remove_this_window_action);
    menu->addAction(set_window_index_action);
    menu->addSeparator();
    menu->addAction(refresh_size_action);
    menu->addAction(set_size_action);
    menu->addSeparator();
    style_set_menu->addAction(set_text_font_action);
    style_set_menu->addAction(set_text_color_action);
    style_set_menu->addAction(set_status_color_active_action);
    style_set_menu->addAction(set_status_color_action);
    style_set_menu->addAction(set_container_height_action);
    style_set_menu->addAction(set_tips_text_font_action);
    style_set_menu->addAction(set_tips_text_color_action);
    menu->addMenu(style_set_menu);
    menu->addAction(save_action);
    menu->addAction(load_action);
    setFixedSize(50, 24);
    resize(50, 24);
}
void Plugin_Item_Widget::contextMenuEvent(QContextMenuEvent *event)
{
    QAction *know_what = menu->exec(mapToGlobal(event->pos()));
    if (know_what == add_window_action)
    {
        emit Add_Window();
    }
    else if (know_what == remove_this_window_action)
    {
        for (int i = 0; i < windowitem_list_ptr->count(); ++i)
        {
            if (windowitem_list_ptr->at(i)->geometry().contains(event->pos()))
            {
                auto *ptr = windowitem_list_ptr->at(i);
                windowitem_list_ptr->removeAt(i);
                ptr->deleteLater();
                fresh_size();
                emit remove_window();
                return;
            }
        }
    }
    else if (know_what == set_window_index_action)
    {
        for (int i = 0; i < windowitem_list_ptr->count(); ++i)
        {
            if (windowitem_list_ptr->at(i)->geometry().contains(event->pos()))
            {
                bool ok = false;
                int move_to_index = QInputDialog::getInt(nullptr, tr("设置索引"), tr("移动到索引(0始):"), i, 0, windowitem_list_ptr->count() - 1, 1, &ok);
                if (ok && i != move_to_index)
                {
                    windowitem_list_ptr->move(i, move_to_index);
                }

                fresh_size();
                return;
            }
        }
    }
    else if (know_what == show_window_action)
    {
        for (int i = 0; i < windowitem_list_ptr->count(); ++i)
        {
            if (windowitem_list_ptr->at(i)->geometry().contains(event->pos()))
            {
                windowitem_list_ptr->at(i)->show_window_action();
                return;
            }
        }
    }
    else if (know_what == hide_window_action)
    {
        for (int i = 0; i < windowitem_list_ptr->count(); ++i)
        {
            if (windowitem_list_ptr->at(i)->geometry().contains(event->pos()))
            {
                windowitem_list_ptr->at(i)->hide_window_action();
                return;
            }
        }
    }
    else if (know_what == max_window_size_action)
    {
        for (int i = 0; i < windowitem_list_ptr->count(); ++i)
        {
            if (windowitem_list_ptr->at(i)->geometry().contains(event->pos()))
            {
                windowitem_list_ptr->at(i)->max_window_size_action();
                return;
            }
        }
    }
    else if (know_what == normal_window_size_action)
    {
        for (int i = 0; i < windowitem_list_ptr->count(); ++i)
        {
            if (windowitem_list_ptr->at(i)->geometry().contains(event->pos()))
            {
                windowitem_list_ptr->at(i)->normal_window_size_action();
                return;
            }
        }
    }
    else if (know_what == close_window_action)
    {
        for (int i = 0; i < windowitem_list_ptr->count(); ++i)
        {
            if (windowitem_list_ptr->at(i)->geometry().contains(event->pos()))
            {
                windowitem_list_ptr->at(i)->close_window_action();
                return;
            }
        }
    }
    else if (know_what == set_size_action)
    {
        for (int i = 0; i < windowitem_list_ptr->count(); ++i)
        {
            if (windowitem_list_ptr->at(i)->geometry().contains(event->pos()))
            {
                windowitem_list_ptr->at(i)->set_size_action();
                return;
            }
        }
    }
    else if (know_what == refresh_size_action)
    {
        emit remove_window();
        return;
    }
    else if (know_what == set_text_font_action)
    {
        bool ok = false;
        QFont font = QFontDialog::getFont(&ok, Public_Config::text_font, nullptr);
        if (!ok)
        {
            return;
        }
        Public_Config::text_font = font;
    }
    else if (know_what == set_text_color_action)
    {
        QColorDialog colorDialog;
        colorDialog.setOption(QColorDialog::ShowAlphaChannel);
        colorDialog.setCurrentColor(Public_Config::text_color);
        colorDialog.setParent(nullptr);
        colorDialog.setWindowTitle("获取颜色");
        if (colorDialog.exec() != QDialog::Accepted)
        {
            return;
        }
        QColor new_color = colorDialog.currentColor();
        Public_Config::text_color = new_color;
    }
    else if (know_what == set_status_color_active_action)
    {
        QColorDialog colorDialog;
        colorDialog.setOption(QColorDialog::ShowAlphaChannel);
        colorDialog.setCurrentColor(Public_Config::status_active_color);
        colorDialog.setParent(nullptr);
        colorDialog.setWindowTitle("获取颜色");
        if (colorDialog.exec() != QDialog::Accepted)
        {
            return;
        }
        QColor new_color = colorDialog.currentColor();
        Public_Config::status_active_color = new_color;
    }
    else if (know_what == set_status_color_action)
    {
        QColorDialog colorDialog;
        colorDialog.setOption(QColorDialog::ShowAlphaChannel);
        colorDialog.setCurrentColor(Public_Config::status_color);
        colorDialog.setParent(nullptr);
        colorDialog.setWindowTitle("获取颜色");
        if (colorDialog.exec() != QDialog::Accepted)
        {
            return;
        }
        QColor new_color = colorDialog.currentColor();
        Public_Config::status_color = new_color;
    }
    else if (know_what == set_tips_text_font_action)
    {
        bool ok = false;
        QFont font = QFontDialog::getFont(&ok, Public_Config::tips_text_font, nullptr);
        if (!ok)
        {
            return;
        }
        Public_Config::tips_text_font = font;
    }
    else if (know_what == set_tips_text_color_action)
    {
        QColorDialog colorDialog;
        colorDialog.setOption(QColorDialog::ShowAlphaChannel);
        colorDialog.setCurrentColor(Public_Config::tips_text_color);
        colorDialog.setParent(nullptr);
        colorDialog.setWindowTitle("获取颜色");
        if (colorDialog.exec() != QDialog::Accepted)
        {
            return;
        }
        QColor new_color = colorDialog.currentColor();
        Public_Config::tips_text_color = new_color;
    }
    else if (know_what == set_container_height_action)
    {
        bool ok = false;
        int new_out_line = QInputDialog::getInt(nullptr, tr("获取数值"), tr("输入高度:"), Public_Config::container_height, 0, 2147483647, 1, &ok);
        if (!ok)
        {
            return;
        }
        Public_Config::container_height = new_out_line;
    }
    else if (know_what == load_action)
    {
        Public_Config::load();
    }
    else if (know_what == save_action)
    {
        Public_Config::save();
    }
    else
    {
        if (menu_sender)
        {
            menu_sender->Send_Ptr(know_what);
        }
    }
}
void Plugin_Item_Widget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
    if (event->button() == Qt::MouseButton::LeftButton)
    if (mouse_move_pos != mapToGlobal(event->pos()))
    {
        for (int i = 0; i < windowitem_list_ptr->count(); ++i)
        {
            if (windowitem_list_ptr->at(i)->geometry().contains(event->pos()))
            {
                windowitem_list_ptr->at(i)->click_window_action();
                for (int j = 0; j < windowitem_list_ptr->count(); ++j)
                {
                    windowitem_list_ptr->at(j)->checkWindowStatus();
                }
                return;
            }
        }
    }
}
void Plugin_Item_Widget::mouseMoveEvent(QMouseEvent *event)
{
    mouse_move_pos = mapToGlobal(event->pos());
    QWidget::mouseMoveEvent(event);
}
void Plugin_Item_Widget::set_carrier_menu(QMenu *carrier_menu, P_Sender * const m_menu_sender)
{
    if (item_carrier_menu)
    {
        menu->removeAction(item_carrier_menu);
        item_carrier_menu = nullptr;
    }
    menu_sender = m_menu_sender;
    item_carrier_menu = menu->addMenu(carrier_menu);
}
void Plugin_Item_Widget::fresh_size()
{
    if ((*pos_ptr == 0 || *pos_ptr == 2))
    {
        for (int i = 0; i < windowitem_list_ptr->count(); ++i)
        {
            if (!windowitem_list_ptr->at(i)->isWindowExists())
            {
                auto *ptr = windowitem_list_ptr->at(i);
                windowitem_list_ptr->removeAt(i);
                ptr->deleteLater();
                i--;
                emit remove_window();
                continue;
            }
            if (i == 0)
            {
                windowitem_list_ptr->at(i)->move(0, 0);
            }
            else
            {
                windowitem_list_ptr->at(i)->move(windowitem_list_ptr->at(i - 1)->geometry().right() + 3, 0);
            }
        }
    }
    else
    {
        for (int i = 0; i < windowitem_list_ptr->count(); ++i)
        {
            if (!windowitem_list_ptr->at(i)->isWindowExists())
            {
                auto *ptr = windowitem_list_ptr->at(i);
                windowitem_list_ptr->removeAt(i);
                ptr->deleteLater();
                i--;
                emit remove_window();
                continue;
            }
            if (i == 0)
            {
                windowitem_list_ptr->at(i)->move(0, 0);
            }
            else
            {
                windowitem_list_ptr->at(i)->move(0, windowitem_list_ptr->at(i - 1)->geometry().bottom() + 3);
            }
        }
    }
    if (!windowitem_list_ptr->isEmpty())
    {
        QPoint BR_pos = windowitem_list_ptr->last()->geometry().bottomRight();
        setFixedSize(BR_pos.x(), BR_pos.y());
        resize(BR_pos.x(), BR_pos.y());
    }
    else
    {
        setFixedSize(50, Public_Config::container_height);
        resize(50, Public_Config::container_height);
    }
}
QSize Plugin_Item_Widget::sizeHint() const//这是为dde-dock准备的
{
    if ((*pos_ptr == 0 || *pos_ptr == 2))
    {
        for (int i = 0; i < windowitem_list_ptr->count(); ++i)
        {
            if (i == 0)
            {
                windowitem_list_ptr->at(i)->move(0, 0);
            }
            else
            {
                windowitem_list_ptr->at(i)->move(windowitem_list_ptr->at(i - 1)->geometry().right() + 3, 0);
            }
        }
    }
    else
    {
        for (int i = 0; i < windowitem_list_ptr->count(); ++i)
        {
            if (i == 0)
            {
                windowitem_list_ptr->at(i)->move(0, 0);
            }
            else
            {
                windowitem_list_ptr->at(i)->move(0, windowitem_list_ptr->at(i - 1)->geometry().bottom() + 3);
            }
        }
    }
    if (!windowitem_list_ptr->isEmpty())
    {
        QPoint BR_pos = windowitem_list_ptr->last()->geometry().bottomRight();
        return QSize(BR_pos.x(), BR_pos.y());
    }
    else
    {
        return QSize(50, Public_Config::container_height);
    }
}
