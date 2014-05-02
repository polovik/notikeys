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
                     << "Dependencies count:" << metaData.value(NS_PLUGIN_INFO::fieldDependencies).toArray().size()
                     << "UID:" << metaData.value(NS_PLUGIN_INFO::fieldID).toString();
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

QStringList PluginsManager::activePlugins()
{
    QStringList ids;
    ids << "1";
    return ids;
}

QStringList PluginsManager::inactivePlugins()
{
    QStringList ids;
    return ids;
}

//------------------------------------------------------------------------------

PluginsImageProvider::PluginsImageProvider(PluginsManager *manager) : QQuickImageProvider(QQuickImageProvider::Image)
{
    m_manager = manager;
}

QImage PluginsImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize);
    qDebug() << "requestImage" << id << requestedSize;
    QStringList path = id.split("/");
    if (path.count() != 3) {
        qCritical() << "Incorrect reference to image:" << id;
        Q_ASSERT(false);
        return QImage();
    }

//    ContentManager::contentType type = static_cast<ContentManager::contentType>(path[0].toInt());
//    if ((type != ContentManager::CONTENT_GAMES) && (type != ContentManager::CONTENT_MOVIES)) {
//        qCritical() << "Unknown content's type" << type;
//        Q_ASSERT(false);
//        return QImage();
//    }
//    int contentId = path[2].toInt();
//    if (path[1] == "icon") {
//        QImage icon = m_manager->getIcon(contentId, type);
//        *size = icon.size();
//        return icon;
//    } else if (path[1] == "poster") {
//        QImage poster = m_manager->getPoster(contentId, type);
//        *size = poster.size();
//        return poster;
//    } else {
//        qCritical() << "Unknown image type" << path[1];
//        Q_ASSERT(false);
//        return QImage();
//    }

    return QImage();
}

QQuickImageProvider::ImageType PluginsImageProvider::imageType() const
{
    return QQuickImageProvider::Image;
}
