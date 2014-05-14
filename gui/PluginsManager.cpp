#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QPluginLoader>
#include <QApplication>
#include <QJsonObject>
#include <QJsonArray>
#include <QQmlContext>
#include "PluginsManager.h"
#include "PluginInterface.h"

PluginsManager::PluginsManager(QQmlContext *ctx, QObject *parent) :
    QObject(parent), m_qmlContext(ctx)
{
}

const QImage PluginsManager::getPluginLogo(QString uid) const
{
    if (!m_plugins.contains(uid)) {
        qCritical() << "Request to unloaded plugin" << uid;
        Q_ASSERT(false);
        return QImage();
    }

    PluginInfo *info = m_plugins.value(uid);
    const QJsonObject& metaData = info->m_metaData;
    QString fileName = metaData.value(NS_PLUGIN_INFO::fieldLogoFile).toString();

    QFileInfo pluginFile(info->m_absoluteFilePath);
    QFileInfo fi(pluginFile.absoluteDir(), fileName);
    if (!fi.exists()) {
        qCritical() << "File" << fileName << "doesn't present in folder" << pluginFile.absoluteDir();
        Q_ASSERT(false);
        return QImage();
    }

    QImage logo(fi.absoluteFilePath());
    return logo;
}

bool PluginsManager::loadPlugins()
{
    m_plugins.clear(); // TODO clear each instance of PluginInfo *
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
            QString title = metaData.value(NS_PLUGIN_INFO::fieldTitle).toString();
            qDebug() << "Plugin info: Title:" << title
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
                    plugin->loadPlugin();
                    plugin->exportToQML(m_qmlContext);
                    QString settingsfileName = metaData.value(NS_PLUGIN_INFO::fieldSettingsFile).toString();
                    QFileInfo fi(pluginDir, settingsfileName);
                    if (!fi.exists()) {
                        qCritical() << "File" << settingsfileName << "doesn't present in folder" << pluginDir;
                        Q_ASSERT(false);
                        continue;
                    }
                    PluginInfo *info = new PluginInfo();
                    info->setActive(false);
                    info->m_plugin = plugin;
                    info->m_absoluteFilePath = pluginPath;
                    info->m_uid = uid;
                    info->m_name = title;
                    info->m_settingsScreenPath = fi.absoluteFilePath();
                    info->m_metaData = metaData;
                    m_plugins.insert(uid, info);
                } else {
                    qWarning() << "Plugin" << fileName << "has unknown interface";
                }
            }
        }
    }

    updatePluginsModel();
    return true;
}

void PluginsManager::startActivePlugins()
{
    foreach (const QString &uid, m_plugins.keys()) {
        PluginInfo *info = m_plugins.value(uid);
        if (info->active())
            info->m_plugin->start();
    }
}

void PluginsManager::stopPlugins()
{
    foreach (const QString &uid, m_plugins.keys()) {
        PluginInfo *info = m_plugins.value(uid);
        info->m_plugin->stop();
    }
}

void PluginsManager::activatePlugin(QString uid)
{
    if (!m_plugins.contains(uid)) {
        qCritical() << "Request to unloaded plugin" << uid;
        Q_ASSERT(false);
        return;
    }

    PluginInfo *info = m_plugins.value(uid);
    info->setActive(true);
    info->m_plugin->start();
    updatePluginsModel();
}

void PluginsManager::updatePluginsModel()
{
    QList<QObject*> pluginsList;
    foreach (const QString &uid, m_plugins.keys()) {
        pluginsList.append(m_plugins.value(uid));
    }
    m_qmlContext->setContextProperty("pluginsModel", QVariant::fromValue(pluginsList));
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
