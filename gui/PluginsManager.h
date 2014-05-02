#ifndef PLUGINSMANAGER_H
#define PLUGINSMANAGER_H

#include <QObject>
#include <QQuickImageProvider>

class PluginsManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginsManager(QObject *parent = 0);

signals:

public slots:
    bool loadPlugins();
    QStringList activePlugins();
    QStringList inactivePlugins();
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
