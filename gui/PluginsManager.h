#ifndef PLUGINSMANAGER_H
#define PLUGINSMANAGER_H

#include <QObject>
#include <QMap>
#include <QQuickImageProvider>
#include <QJsonObject>

class PluginInterface;
class QTranslator;

class PluginInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString uid READ uid NOTIFY uidChanged)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString settingsScreenPath READ settingsScreenPath)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(int pos READ pos WRITE setPos)

public:
    bool active() const
    { return m_active; }
    void setActive(bool active)
    { m_active = active; }
    int pos() const
    { return m_pos; }
    void setPos(int pos)
    { m_pos = pos; }
    QString name() const
    { return m_name; }

    PluginInterface *m_plugin;
    QString m_absoluteFilePath;
    QString m_uid;
    QString m_name;
    QString m_settingsScreenPath;
    QJsonObject m_metaData;
    QTranslator *m_translator;

signals:
    void uidChanged(QString uid);
    void activeChanged(bool active);

private:
    QString uid() const
    { return m_uid; }
    QString settingsScreenPath() const
    { return m_settingsScreenPath; }

    bool m_active;
    int m_pos;
};

class PluginsManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginsManager(QQmlContext *ctx, QObject *parent = 0);
    const QImage getPluginLogo(QString uid) const;

public slots:
    bool loadPlugins();
    void startActivePlugins();
    void stopPlugins();
    void activatePlugin(QString uid, int pos);
    void deactivatePlugin(QString uid, int pos);
    void loadLanguage(QString lang);
    void processExternalEvents(qint32 pluginUid, qint32 eventsCount);
    void processButtonPressing(QString pluginUid);

private:
    void updatePluginsModel();

    QQmlContext *m_qmlContext;
    QMap<QString, PluginInfo *> m_plugins; // key - UID, value - reference to plugin's description
    QTranslator *m_mainMenuTranslator;

signals:
};

//------------------------------------------------------------------------------

class PluginsImageProvider : public QQuickImageProvider
{
public:
    PluginsImageProvider(PluginsManager *manager);

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
    QQuickImageProvider::ImageType imageType() const;

private:
    PluginsManager *m_manager;
};

#endif // PLUGINSMANAGER_H
