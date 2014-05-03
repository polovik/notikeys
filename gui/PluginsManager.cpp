#include <QDebug>
#include <QDir>
#include <QFileInfo>
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

const QImage PluginsManager::getPluginLogo(QString uid) const
{
    if (!m_plugins.contains(uid)) {
        qCritical() << "Request to unloaded plugin" << uid;
        Q_ASSERT(false);
        return QImage();
    }

    QPluginLoader pluginLoader(m_plugins.value(uid));
    const QJsonObject& metaData = pluginLoader.metaData().value("MetaData").toObject();
    QString fileName = metaData.value(NS_PLUGIN_INFO::fieldLogoFile).toString();

    QString dir = pluginDir(uid);
    QFileInfo fi(QDir(dir), fileName);
    if (!fi.exists()) {
        qCritical() << "File" << fileName << "doesn't present in folder" << dir;
        Q_ASSERT(false);
        return QImage();
    }

    QImage logo(fi.absoluteFilePath());
    return logo;
}

bool PluginsManager::loadPlugins()
{
    m_plugins.clear();
    QDir pluginsDir(qApp->applicationDirPath());
    bool dirExist = false;
#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        dirExist = pluginsDir.cd("build_plugins");
    } else {
        dirExist = pluginsDir.cd("plugins");
    }
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    if (!dirExist) {
        qCritical() << "Folder \"plugins\" is absent. Expect in" << pluginsDir.absolutePath();
        return false;
    }

    qDebug() << "Load plugins from folder" << pluginsDir.absolutePath();
    foreach (QString folder, pluginsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QDir pluginDir(pluginsDir.absoluteFilePath(folder));
        foreach (QString fileName, pluginDir.entryList(QDir::Files)) {
            QString pluginPath = pluginDir.absoluteFilePath(fileName);
            QPluginLoader pluginLoader(pluginPath);
            const QJsonObject& metaData = pluginLoader.metaData().value("MetaData").toObject();
            QString uid = metaData.value(NS_PLUGIN_INFO::fieldID).toString();
            qDebug() << "Plugin info: Title:" << metaData.value(NS_PLUGIN_INFO::fieldTitle).toString()
                     << "Version:" << metaData.value(NS_PLUGIN_INFO::fieldVersion).toString()
                     << "Description:" << metaData.value(NS_PLUGIN_INFO::fieldDescription).toString()
                     << "Dependencies count:" << metaData.value(NS_PLUGIN_INFO::fieldDependencies).toArray().size()
                     << "UID:" << uid;
            QObject *pluginInstance = pluginLoader.instance();
            if (pluginInstance) {
                PluginInterface *plugin = qobject_cast<PluginInterface *>(pluginInstance);
                if (plugin) {
                    if (uid.isEmpty() || m_plugins.contains(uid)) {
                        qCritical() << "Incorrect UID for plugin" << fileName << "-" << uid;
                        continue;
                    }
                    qDebug() << "Plugin" << metaData.value(NS_PLUGIN_INFO::fieldTitle).toString() << "has been successfully loaded";
                    m_plugins.insert(uid, pluginPath);
                } else {
                    qWarning() << "Plugin" << fileName << "has unknown interface";
                }
            }
        }
    }

    return true;
}

QString PluginsManager::getSettingsScreenPath(QString uid) const
{
    if (!m_plugins.contains(uid)) {
        qCritical() << "Request to unloaded plugin" << uid;
        Q_ASSERT(false);
        return "";
    }

    QPluginLoader pluginLoader(m_plugins.value(uid));
    const QJsonObject& metaData = pluginLoader.metaData().value("MetaData").toObject();
    QString fileName = metaData.value(NS_PLUGIN_INFO::fieldSettingsFile).toString();

    QString dir = pluginDir(uid);
    QFileInfo fi(QDir(dir), fileName);
    if (!fi.exists()) {
        qCritical() << "File" << fileName << "doesn't present in folder" << dir;
        Q_ASSERT(false);
        return "";
    }

    return fi.absoluteFilePath();
}

QString PluginsManager::getTitle(QString uid) const
{
    if (!m_plugins.contains(uid)) {
        qCritical() << "Request to unloaded plugin" << uid;
        Q_ASSERT(false);
        return "";
    }

    QPluginLoader pluginLoader(m_plugins.value(uid));
    const QJsonObject& metaData = pluginLoader.metaData().value("MetaData").toObject();
    QString title = metaData.value(NS_PLUGIN_INFO::fieldTitle).toString();
    return title;
}

PluginInterface *PluginsManager::pluginInterface(QString uid) const
{
    if (!m_plugins.contains(uid)) {
        qCritical() << "Request to unloaded plugin" << uid;
        Q_ASSERT(false);
        return NULL;
    }

    QPluginLoader pluginLoader(m_plugins.value(uid));
    QObject *pluginInstance = pluginLoader.instance();
    if (!pluginInstance) {
        qCritical() << "Unknown file was insert in list of plugins:" << m_plugins.value(uid);
        Q_ASSERT(false);
        return NULL;
    }

    PluginInterface *plugin = qobject_cast<PluginInterface *>(pluginInstance);
    if (!plugin) {
        qCritical() << "Incorrect plugin was insert in list of plugins:" << m_plugins.value(uid);
        Q_ASSERT(false);
        return NULL;
    }

    return plugin;
}

QString PluginsManager::pluginDir(QString uid) const
{
    if (!m_plugins.contains(uid)) {
        qCritical() << "Request to unloaded plugin" << uid;
        Q_ASSERT(false);
        return "";
    }

    QFileInfo fi(m_plugins.value(uid));
    return fi.absolutePath();
}

//------------------------------------------------------------------------------

PluginsImageProvider::PluginsImageProvider(PluginsManager *manager) : QQuickImageProvider(QQuickImageProvider::Image)
{
    m_manager = manager;
}

QImage PluginsImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize);

    QImage icon = m_manager->getPluginLogo(id);
    *size = icon.size();
    return icon;
}

QQuickImageProvider::ImageType PluginsImageProvider::imageType() const
{
    return QQuickImageProvider::Image;
}
