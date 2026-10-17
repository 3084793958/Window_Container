QT += core widgets
LIBS += -lX11 -lXtst
TEMPLATE = lib
CONFIG += plugin
TARGET = Window_Container
DISTFILES += \
    Window_Container.json

HEADERS += \
    interfaces/easy_desktop_class.h \
    window_container.h \
    plugin_item_widget.h \
    windowitem.h \
    public_config.h \
    interfaces/dde-dock/v1.2.3/constants.h \
    interfaces/dde-dock/v1.2.3/pluginproxyinterface.h \
    interfaces/dde-dock/v1.2.3/pluginsiteminterface.h

SOURCES += \
    interfaces/easy_desktop_class.cpp \
    window_container.cpp \
    plugin_item_widget.cpp \
    windowitem.cpp \
    public_config.cpp \
    interfaces/dde-dock/v1.2.3/pluginsiteminterface.cpp
