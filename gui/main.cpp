#include <QDebug>
#include <QQmlContext>
#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"
#include "PluginsManager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QtQuick2ApplicationViewer viewer;

    PluginsManager pluginsManager;
    pluginsManager.loadPlugins();
    viewer.rootContext()->setContextProperty("PluginsManager", &pluginsManager);

    QQmlImageProviderBase *imageProvider = new PluginsImageProvider(&pluginsManager);
    viewer.engine()->addImageProvider(QLatin1String("pluginlogo"), imageProvider);

    viewer.setMainQmlFile(QStringLiteral("qml/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
