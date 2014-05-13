#ifndef PLUGINSMANAGER_H
#define PLUGINSMANAGER_H

#include <QObject>
#include <QMap>
#include <QQuickImageProvider>
#include <QJsonObject>

class PluginInterface;

class PluginInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString uid READ uid NOTIFY uidChanged)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString settingsScreenPath READ settingsScreenPath)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

public:
    PluginInterface *m_plugin;
    QString m_absoluteFilePath;
    QString m_uid;
    QString m_name;
    QString m_settingsScreenPath;
    QJsonObject m_metaData;

signals:
    void uidChanged(QString uid);
    void activeChanged(bool active);

private:
    QString uid() const
    { return m_uid; }
    QString name() const
    { return m_name; }
    QString settingsScreenPath() const
    { return m_settingsScreenPath; }
    bool active() const
    { return m_active; }
    void setActive(bool active)
    { m_active = active; }

    bool m_active;
};

class PluginsManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginsManager(QQmlContext *ctx, QObject *parent = 0);
    const QImage getPluginLogo(QString uid) const;

signals:

public slots:
    bool loadPlugins();

private:
    QQmlContext *m_qmlContext;

    QMap<QString, PluginInfo *> m_plugins; // key - UID, value - reference to plugin's description
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
