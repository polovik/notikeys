#ifndef PLUGINSMANAGER_H
#define PLUGINSMANAGER_H

#include <QObject>
#include <QMap>
#include <QQuickImageProvider>

class PluginInterface;

class PluginsManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginsManager(QQmlContext *ctx, QObject *parent = 0);
    const QImage getPluginLogo(QString uid) const;

signals:

public slots:
    bool loadPlugins();
    QString getSettingsScreenPath(QString uid) const;
    QString getTitle(QString uid) const;

private:
    PluginInterface *pluginInterface(QString uid) const;
    QString pluginDir(QString uid) const;
    QQmlContext *m_qmlContext;

    QMap<QString, QString> m_plugins; // key - UID, value - file path
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
