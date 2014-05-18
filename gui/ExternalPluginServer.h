#ifndef EXTERNALPLUGINSERVER_H
#define EXTERNALPLUGINSERVER_H

#include <QObject>
#include <QMultiMap>
#include <QString>

class QLocalServer;
class QLocalSocket;

class ExternalPluginServer : public QObject
{
    Q_OBJECT
public:
    explicit ExternalPluginServer(QObject *parent = 0);

public slots:
    bool startServer();

private slots:
    void storeNewConnection();
    void readPluginData();
    void forgetConnection();

private:
    QLocalServer *m_server;
    QMultiMap<QString, QLocalSocket *> m_pluginConnections; // key - plugins UID, value - connection to plugin

signals:

};

#endif // EXTERNALPLUGINSERVER_H
