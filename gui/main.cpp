#include <QDebug>
#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"
#include "PluginsManager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    PluginsManager pluginsManager;
    pluginsManager.loadPlugins();

    QtQuick2ApplicationViewer viewer;
    viewer.setMainQmlFile(QStringLiteral("qml/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
