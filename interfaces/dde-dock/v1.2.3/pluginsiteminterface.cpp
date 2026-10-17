#include "pluginsiteminterface.h"

#include <QFont>

PluginsItemInterface::~PluginsItemInterface()
{}
const QString PluginsItemInterface::pluginDisplayName() const
{
    return QString();
}
QWidget *PluginsItemInterface::itemTipsWidget(const QString &itemKey)
{
    Q_UNUSED(itemKey);
    return nullptr;
}
QWidget *PluginsItemInterface::itemPopupApplet(const QString &itemKey)
{
    Q_UNUSED(itemKey);
    return nullptr;
}
const QString PluginsItemInterface::itemCommand(const QString &itemKey)
{
    Q_UNUSED(itemKey);
    return QString();
}
const QString PluginsItemInterface::itemContextMenu(const QString &itemKey)
{
    Q_UNUSED(itemKey);
    return QString();
}
void PluginsItemInterface::invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked)
{
    Q_UNUSED(itemKey);
    Q_UNUSED(menuId);
    Q_UNUSED(checked);
}
int PluginsItemInterface::itemSortKey(const QString &itemKey)
{
    Q_UNUSED(itemKey);
    return 1;
}
void PluginsItemInterface::setSortKey(const QString &itemKey, const int order)
{
    Q_UNUSED(itemKey);
    Q_UNUSED(order);
}
bool PluginsItemInterface::itemAllowContainer(const QString &itemKey)
{
    Q_UNUSED(itemKey);
    return false;
}
bool PluginsItemInterface::itemIsInContainer(const QString &itemKey)
{
    Q_UNUSED(itemKey);
    return false;
}
void PluginsItemInterface::setItemIsInContainer(const QString &itemKey, const bool container)
{
    Q_UNUSED(itemKey);
    Q_UNUSED(container);
}

bool PluginsItemInterface::pluginIsAllowDisable()
{
    return false;
}
bool PluginsItemInterface::pluginIsDisable()
{
    return false;
}
void PluginsItemInterface::pluginStateSwitched()
{}
void PluginsItemInterface::displayModeChanged(const Dock::DisplayMode displayMode)
{
    Q_UNUSED(displayMode);
}
void PluginsItemInterface::positionChanged(const Dock::Position position)
{
    Q_UNUSED(position);
}
void PluginsItemInterface::refreshIcon(const QString &itemKey)
{
    Q_UNUSED(itemKey);
}
Dock::DisplayMode PluginsItemInterface::displayMode() const
{
    return qApp->property(PROP_DISPLAY_MODE).value<Dock::DisplayMode>();
}
Dock::Position PluginsItemInterface::position() const
{
    return qApp->property(PROP_POSITION).value<Dock::Position>();
}
void PluginsItemInterface::pluginSettingsChanged()
{}
PluginsItemInterface::PluginType PluginsItemInterface::type()
{
    return Normal;
}
PluginsItemInterface::PluginSizePolicy PluginsItemInterface::pluginSizePolicy() const
{
    return System;
}
bool PluginsItemInterface::pluginIsAllowUnload()
{
    return false;
}
void PluginsItemInterface::pluginUnload()
{}
void PluginsItemInterface::pluginGetSender(P_Sender * const update_sender, P_Sender * const send_data_sender)
{
    (void) update_sender;
    (void) send_data_sender;
}
QPair<int, int> PluginsItemInterface::pluginSetCarrierSpacing(PluginsItemInterface::Carrier_Type carrier_type, bool *isValid)
{
    (void) carrier_type;
    *isValid = false;
    return qMakePair(0, 0);
}
void PluginsItemInterface::pluginGetCarrierSpacing(PluginsItemInterface::Carrier_Type carrier_type, int distance_width, int distance_height)
{
    (void) carrier_type;
    (void) distance_width;
    (void) distance_height;
}
QPair<int, int> PluginsItemInterface::pluginSetCarrierOffset(PluginsItemInterface::Carrier_Type carrier_type, bool *isValid)
{
    (void) carrier_type;
    *isValid = false;
    return qMakePair(0, 0);
}
void PluginsItemInterface::pluginGetCarrierOffset(PluginsItemInterface::Carrier_Type carrier_type, int delta_x, int delta_y)
{
    (void) carrier_type;
    (void) delta_x;
    (void) delta_y;
}
QColor PluginsItemInterface::pluginSetCarrierColor(PluginsItemInterface::Carrier_Type carrier_type, bool *isValid)
{
    (void) carrier_type;
    *isValid = false;
    return QColor();
}
void PluginsItemInterface::pluginGetCarrierColor(PluginsItemInterface::Carrier_Type carrier_type, QColor background_color)
{
    (void) carrier_type;
    (void) background_color;
}
int PluginsItemInterface::pluginSetCarrierRadius(PluginsItemInterface::Carrier_Type carrier_type, bool *isValid)
{
    (void) carrier_type;
    *isValid = false;
    return 0;
}
void PluginsItemInterface::pluginGetCarrierRadius(PluginsItemInterface::Carrier_Type carrier_type, int radius)
{
    (void) carrier_type;
    (void) radius;
}
QSize PluginsItemInterface::pluginSetCarrierSize(PluginsItemInterface::Carrier_Type carrier_type, bool *isValid)
{
    (void) carrier_type;
    *isValid = false;
    return QSize();
}
void PluginsItemInterface::pluginGetCarrierQMenu(PluginsItemInterface::Carrier_Type carrier_type, QMenu *menu, P_Sender * const action_sender)
{
    (void) carrier_type;
    (void) menu;
    (void) action_sender;
}
QPair<QMenu *, P_Sender * const> PluginsItemInterface::pluginAddToCarrierQMenu(PluginsItemInterface::Carrier_Type carrier_type, bool *isValid)
{
    (void) carrier_type;
    *isValid = false;
    return QPair<QMenu *, P_Sender * const>(nullptr, nullptr);
}
QSize PluginsItemInterface::pluginSetWidgetSize(PluginsItemInterface::Carrier_Type carrier_type, bool *isValid)
{
    (void) carrier_type;
    *isValid = false;
    return QSize();
}
QPoint PluginsItemInterface::pluginSetCarrierPos(PluginsItemInterface::Carrier_Type carrier_type, QPoint item_pos, QSize item_size, bool *isValid)
{
    (void) carrier_type;
    (void) item_pos;
    (void) item_size;
    *isValid = false;
    return QPoint();
}
int PluginsItemInterface::pluginSetCarrierPage(PluginsItemInterface::Carrier_Type carrier_type, int total_page, bool *isValid)
{
    (void) carrier_type;
    (void) total_page;
    *isValid = false;
    return 0;
}
void PluginsItemInterface::pluginGetIsMouseInPluginCarrier(PluginsItemInterface::Carrier_Type carrier_type, bool result)//用于给music-island打补丁
{
    (void) carrier_type;
    (void) result;
}
void PluginsItemInterface::pluginGetShowCarrierCloseButton(PluginsItemInterface::Carrier_Type carrier_type, bool result)
{
    (void) carrier_type;
    (void) result;
}
bool PluginsItemInterface::pluginSetShowCarrierCloseButton(PluginsItemInterface::Carrier_Type carrier_type, bool *isValid)
{
    (void) carrier_type;
    *isValid = false;
    return false;
}
void PluginsItemInterface::Trans_Update()
{}
void PluginsItemInterface::update_style_V2(QColor m_theme_color, QColor m_theme_background_color, QColor m_theme_text_color, QColor m_select_text_color, QColor m_disabled_text_color, const QString &m_checked_icon_path, const QString &m_checked_and_selected_icon_path, int m_radius, int m_icon_size, QColor m_separator_color, int m_min_width, int m_on_blurring_type, QFont m_text_font, QColor m_menu_outline_color, int m_menu_outline_width, int m_blur_radius, QColor m_highlight_color)
{
    (void) m_theme_color;
    (void) m_theme_background_color;
    (void) m_theme_text_color;
    (void) m_select_text_color;
    (void) m_disabled_text_color;
    (void) m_checked_icon_path;
    (void) m_checked_and_selected_icon_path;
    (void) m_radius;
    (void) m_icon_size;
    (void) m_separator_color;
    (void) m_min_width;
    (void) m_on_blurring_type;
    (void) m_text_font;
    (void) m_menu_outline_color;
    (void) m_menu_outline_width;
    (void) m_blur_radius;
    (void) m_highlight_color;
}
void PluginsItemInterface::call_plugin_save()
{}
void PluginsItemInterface::init_V_0_0_3(const QStringList &loaderNames, const P_Sender *seek_loader_sender, const P_Sender *recive_loader_sender)
{
    (void) loaderNames;
    (void) seek_loader_sender;
    (void) recive_loader_sender;
}

void PluginsItemInterface::easy_desktop_version(const P_Version &Easy_Desktop_Version, const QString &AKA_name)
{
    (void) Easy_Desktop_Version;
    (void) AKA_name;
}
