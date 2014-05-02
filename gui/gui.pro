DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050100

# Add more folders to ship with the application, here
folder_01.source = qml
folder_01.target =
plugins.source = ../build_plugins/*.dll
plugins.target = plugins
DEPLOYMENTFOLDERS = folder_01 plugins

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

QT += widgets

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    PluginsManager.cpp

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    PluginsManager.h \
    PluginInterface.h
