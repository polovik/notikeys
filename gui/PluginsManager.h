#ifndef PLUGINSMANAGER_H
#define PLUGINSMANAGER_H

#include <QObject>

class PluginsManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginsManager(QObject *parent = 0);

signals:

public slots:
    bool loadPlugins();
};

#endif // PLUGINSMANAGER_H
