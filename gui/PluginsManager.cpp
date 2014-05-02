#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QApplication>
#include <QJsonObject>
#include <QJsonArray>
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
    foreach (QString folder, pluginsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QDir pluginDir(pluginsDir.absoluteFilePath(folder));
        foreach (QString fileName, pluginDir.entryList(QDir::Files)) {
            QPluginLoader pluginLoader(pluginDir.absoluteFilePath(fileName));
            const QJsonObject& metaData = pluginLoader.metaData().value("MetaData").toObject();
            qDebug() << "Plugin info: Title:" << metaData.value(NS_PLUGIN_INFO::fieldTitle).toString()
                     << "Version:" << metaData.value(NS_PLUGIN_INFO::fieldVersion).toString()
                     << "Description:" << metaData.value(NS_PLUGIN_INFO::fieldDescription).toString()
                     << "Dependencies count:" << metaData.value(NS_PLUGIN_INFO::fieldDependencies).toArray().size();
            QObject *pluginInstance = pluginLoader.instance();
            if (pluginInstance) {
                PluginInterface *plugin = qobject_cast<PluginInterface *>(pluginInstance);
                if (plugin) {
                    qDebug() << "Plugin" << metaData.value(NS_PLUGIN_INFO::fieldTitle).toString() << "has been successfully loaded";
                } else {
                    qWarning() << "Plugin" << fileName << "has unknown interface";
                }
            }
        }
    }

    return true;
}
