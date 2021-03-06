#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QPluginLoader>
#include <QApplication>
#include <QJsonObject>
#include <QJsonArray>
#include <QQmlContext>
#include <QTranslator>
#include <QtGui/QGuiApplication>
#include "PluginsManager.h"
#include "PluginInterface.h"
#include "../device/Device.h"

PluginsManager::PluginsManager(QQmlContext *ctx, QObject *parent) :
    QObject(parent), m_qmlContext(ctx)
{
    m_mainMenuTranslator = new QTranslator();
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

static void setLedMode(QString pluginUid, LedMode_e mode)
{
//    extern Device g_device;
//    g_device.setLedMode(pluginUid, mode);
}

bool PluginsManager::loadPlugins()
{
    m_plugins.clear(); // TODO clear each instance of PluginInfo * and QTranslator *
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
#else
    dirExist = pluginsDir.cd("plugins");
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
            QObject *pluginInstance = pluginLoader.instance();
            if (pluginInstance) {
                qDebug() << "Plugin info: Title:" << title
                         << "Version:" << metaData.value(NS_PLUGIN_INFO::fieldVersion).toString()
                         << "Description:" << metaData.value(NS_PLUGIN_INFO::fieldDescription).toString()
                         << "Dependencies count:" << metaData.value(NS_PLUGIN_INFO::fieldDependencies).toArray().size()
                         << "UID:" << uid;
                PluginInterface *plugin = qobject_cast<PluginInterface *>(pluginInstance);
                if (plugin) {
                    if (uid.isEmpty() || m_plugins.contains(uid)) {
                        qCritical() << "Incorrect UID for plugin" << fileName << "-" << uid;
                        continue;
                    }
                    qDebug() << "Plugin" << metaData.value(NS_PLUGIN_INFO::fieldTitle).toString() << "has been successfully loaded";
                    plugin->loadPlugin();
                    plugin->exportToQML(m_qmlContext);
                    plugin->setLedControlFunction(&setLedMode, uid);
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
                    info->setPos(-1);
                    info->m_metaData = metaData;
                    info->m_translator = new QTranslator();
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

void PluginsManager::activatePlugin(QString uid, int pos)
{
    if (!m_plugins.contains(uid)) {
        qCritical() << "Request to unloaded plugin" << uid;
        Q_ASSERT(false);
        return;
    }

    PluginInfo *info = m_plugins.value(uid);
    qDebug() << "Activate plugin" << info->m_name;
    info->setActive(true);
    info->setPos(pos);
    info->m_plugin->start();
    updatePluginsModel();
}

void PluginsManager::deactivatePlugin(QString uid, int pos)
{
    // TODO in some cases uid may be invalid, therefore deactivate by pos. todo don't warn about -1 uid at pos
    Q_UNUSED(pos);
    if (!m_plugins.contains(uid)) {
        qWarning() << "Request to unloaded plugin" << uid << "at pos" << pos;
//        Q_ASSERT(false);
        return;
    }

    PluginInfo *info = m_plugins.value(uid);
    qDebug() << "Deactivate plugin" << info->m_name;
    info->setActive(false);
    info->setPos(-1);
    info->m_plugin->stop();
    updatePluginsModel();
}

void PluginsManager::loadLanguage(QString lang)
{
    foreach (const QString &uid, m_plugins.keys()) {
        PluginInfo *info = m_plugins.value(uid);
        const QJsonObject& metaData = info->m_metaData;
        QJsonArray langPacks = metaData.value(NS_PLUGIN_INFO::fieldLanguagePacks).toArray();
        bool loadedTranslatorFile = false;
        foreach (QJsonValue pack, langPacks) {
            QString fileName = pack.toString();
            if (fileName.contains(lang)) {
                QFileInfo pluginFile(info->m_absoluteFilePath);
                QFileInfo fi(pluginFile.absoluteDir(), fileName);
                if (!fi.exists()) {
                    qCritical() << "File" << fileName << "doesn't present in folder" << pluginFile.absoluteDir();
                    Q_ASSERT(false);
                    break;
                }
                QString absFileName = fi.absoluteFilePath();
                absFileName.remove(".qm");
                loadedTranslatorFile = info->m_translator->load(absFileName);
                if (loadedTranslatorFile == false)
                    qWarning() << "Can't load translation" << fi.absoluteFilePath() << absFileName;
                break;
            }
        }
        if (loadedTranslatorFile) {
            QGuiApplication::installTranslator(info->m_translator);
        } else {
            if (!lang.contains("us_US"))
                qWarning() << "Plugin" << info->m_name << "doesn't have language pack";
            QGuiApplication::removeTranslator(info->m_translator);
        }
    }

    QDir mainMenuLangsDir(qApp->applicationDirPath());
    mainMenuLangsDir.cd("langs");
    QStringList nameFilters;
    nameFilters << "*.qm";
    bool loadedFile = false;
    foreach (QString fileName, mainMenuLangsDir.entryList(nameFilters, QDir::Files)) {
        if (fileName.contains(lang)) {
            fileName.remove(".qm");
            loadedFile = m_mainMenuTranslator->load(fileName, "langs");
            if (loadedFile == false)
                qWarning() << "Can't load translation for main menu" << (fileName + QString(".qm"));
            break;
        }
    }
    if (loadedFile)
        QGuiApplication::installTranslator(m_mainMenuTranslator);
    else {
        if (!lang.contains("us_US"))
            qWarning() << "Main menu doesn't have language pack";
        QGuiApplication::removeTranslator(m_mainMenuTranslator);
    }
}

void PluginsManager::processExternalEvents(qint32 pluginUid, qint32 eventsCount)
{
    QString uid = QString::number(pluginUid);
    if (!m_plugins.contains(uid)) {
        qWarning() << "Plugin with uid" << pluginUid << "is missed";
        Q_ASSERT(false);
        return;
    }
    PluginInfo *info = m_plugins.value(uid);
    PluginInterface *interface = info->m_plugin;
    interface->analizeExternalEvents(eventsCount);
}

void PluginsManager::processButtonPressing(QString pluginUid)
{
    if (!m_plugins.contains(pluginUid)) {
        qWarning() << "Plugin with uid" << pluginUid << "is missed";
        Q_ASSERT(false);
        return;
    }
    PluginInfo *info = m_plugins.value(pluginUid);
    PluginInterface *interface = info->m_plugin;
    interface->handleButtonPressing();
}

void PluginsManager::updatePluginsModel()
{
    QList<QObject*> pluginsList;
    qDebug() << "Refresh list of displayed plugins";
    int slot = 0;
    foreach (const QString &uid, m_plugins.keys()) {
        PluginInfo *info = m_plugins.value(uid);
        if (info->pos() == -1) {
            continue;
        }
        qDebug() << "Add plugin" << info->name() << "to slot" << info->pos();
        while (slot < info->pos()) {
            slot++;
            if (!m_plugins.contains("VIRTUAL_-1")) {
                qWarning() << "Plugin for display empty slot is missed";
                continue;
            }
            PluginInfo *info = m_plugins.value("VIRTUAL_-1");
            info->setActive(true);
            pluginsList.append(info);
        }
        pluginsList.append(info);
        slot++;
    }
    while (slot < 3) {
        slot++;
        if (!m_plugins.contains("VIRTUAL_-1")) {
            qWarning() << "Plugin for display empty slot is missed";
            continue;
        }
        PluginInfo *info = m_plugins.value("VIRTUAL_-1");
        info->setActive(true);
        pluginsList.append(info);
    }
    qDebug() << "Number of displayed plugins:" << pluginsList.size();
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
