#ifndef PUBLIC_CONFIG_H
#define PUBLIC_CONFIG_H
#include <QFont>
#include <QColor>
#include <QFontDatabase>
struct Public_Config
{
    static QFont text_font;
    static QColor text_color;
    static QColor status_active_color;
    static QColor status_color;
    static QFont tips_text_font;
    static QColor tips_text_color;
    static int container_height;
    static void save();
    static void load();
    // 新增：宿主主题覆盖（仅内存生效，不写盘）
    static void apply_host_theme(const QFont &font,
                                 const QColor &text,
                                 const QColor &active,
                                 const QColor &inactive);
};

#endif // PUBLIC_CONFIG_H
