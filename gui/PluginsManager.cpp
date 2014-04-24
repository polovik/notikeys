#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QApplication>
#include "PluginsManager.h"
#include "PluginInterface.h"

PluginsManager::PluginsManager(QObject *parent) :
    QObject(parent)
{
}

bool PluginsManager::loadPlugins()
{
    QDir pluginsDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    bool dirExist = pluginsDir.cd("plugins");
    if (!dirExist) {
        qCritical() << "Folder \"plugins\" is absent. Expect in" << pluginsDir.absolutePath();
        return false;
    }

    qDebug() << "Load plugins from folder" << pluginsDir.absolutePath();
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *pluginInstance = pluginLoader.instance();
        if (pluginInstance) {
            PluginInterface *plugin = qobject_cast<PluginInterface *>(pluginInstance);
            if (plugin) {
                qDebug() << "Plugin" << plugin->title() << "has been successfully loaded";
            } else {
                qWarning() << "Plugin" << fileName << "has unknown interface";
            }
        }
    }

    return true;
}
