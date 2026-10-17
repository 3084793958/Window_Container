#include "public_config.h"
#include <QSettings>
#include <QDir>
QFont Public_Config::text_font = QFontDatabase::systemFont(QFontDatabase::TitleFont);
QColor Public_Config::text_color = QColor(255, 255, 255, 255);
QColor Public_Config::status_active_color = QColor(72, 226, 237, 255);
QColor Public_Config::status_color = QColor(140, 141, 142, 255);
QFont Public_Config::tips_text_font = QFontDatabase::systemFont(QFontDatabase::TitleFont);
QColor Public_Config::tips_text_color = QColor(0, 0, 0, 255);
int Public_Config::container_height = 24;

void Public_Config::save()
{
    QSettings settings(QDir::homePath() + "/.local/lib/window_container_plugin/config.ini", QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.clear();
    settings.beginGroup("Main");
    settings.setValue("text_font", text_font);
    settings.setValue("text_color", text_color.rgba());
    settings.setValue("status_active_color", status_active_color.rgba());
    settings.setValue("status_color", status_color.rgba());
    settings.setValue("tips_text_font", tips_text_font);
    settings.setValue("tips_text_color", tips_text_color.rgba());
    settings.setValue("container_height", container_height);
    settings.endGroup();
    settings.sync();
}
void Public_Config::load()
{
    QSettings settings(QDir::homePath() + "/.local/lib/window_container_plugin/config.ini", QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.beginGroup("Main");
    text_font = settings.value("text_font", QFontDatabase::systemFont(QFontDatabase::TitleFont)).value<QFont>();
    text_color = QColor::fromRgba(settings.value("text_color", QColor(255, 255, 255, 255).rgba()).toUInt());
    status_active_color = QColor::fromRgba(settings.value("status_active_color", QColor(72, 226, 237, 255).rgba()).toUInt());
    status_color = QColor::fromRgba(settings.value("status_color", QColor(140, 141, 142, 255).rgba()).toUInt());
    tips_text_font = settings.value("tips_text_font", QFontDatabase::systemFont(QFontDatabase::TitleFont)).value<QFont>();
    tips_text_color = QColor::fromRgba(settings.value("tips_text_color", QColor(0, 0, 0, 255).rgba()).toUInt());
    container_height = settings.value("container_height", 24).toInt();
    settings.endGroup();
}
void Public_Config::apply_host_theme(const QFont &font,
                                     const QColor &text,
                                     const QColor &active,
                                     const QColor &inactive)
{
    if (!font.family().isEmpty()) text_font = font;
    if (text.isValid())           text_color = text;
    if (active.isValid())         status_active_color = active;
    if (inactive.isValid())       status_color = inactive;
}
