QT += core widgets x11extras
LIBS += -lX11 -lXtst
TEMPLATE = lib
CONFIG += plugin
TARGET = Window_Container
DISTFILES += \
    Window_Container.json

HEADERS += \
    interfaces/easy_desktop_class.h \
    interfaces/dde-dock/v1.2.3/constants.h \
    interfaces/dde-dock/v1.2.3/pluginproxyinterface.h \
    interfaces/dde-dock/v1.2.3/pluginsiteminterface.h \
    window_container.h \
    plugin_item_widget.h \
    windowitem.h \
    public_config.h

SOURCES += \
    interfaces/easy_desktop_class.cpp \
    window_container.cpp \
    plugin_item_widget.cpp \
    windowitem.cpp \
    public_config.cpp
