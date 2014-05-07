#include <QDebug>
#include <QQmlContext>
#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"
#include "PluginsManager.h"
#include "Settings.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QtQuick2ApplicationViewer viewer;

    //  Set the organization name and the application name for using QSettings
    app.setOrganizationName("Horns&Hoofs Inc.");
    app.setApplicationName("Notikeys");

    PluginsManager pluginsManager(viewer.rootContext());
    pluginsManager.loadPlugins();
    viewer.rootContext()->setContextProperty("PluginsManager", &pluginsManager);

    Settings settings;
    viewer.rootContext()->setContextProperty("Settings", &settings);

    QQmlImageProviderBase *imageProvider = new PluginsImageProvider(&pluginsManager);
    viewer.engine()->addImageProvider(QLatin1String("pluginlogo"), imageProvider);

    viewer.setMainQmlFile(QStringLiteral("qml/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
