#include <QLocalServer>
#include <QLocalSocket>
#include <QByteArray>
#include <ExternalPluginProtocol.h>
#include "ExternalPluginServer.h"

ExternalPluginServer::ExternalPluginServer(QObject *parent) :
    QObject(parent), m_server(NULL)
{
}

/** @brief Start local Server - it is used Named Pipe for communicate with plugins.
 *  @return true if server succesfully started, otherwise return false.
 */
bool ExternalPluginServer::startServer()
{
    if (m_server != NULL) {
        qCritical() << "Plugins server has been already created";
        Q_ASSERT(false);
    }

    m_server = new QLocalServer;
    if (m_server->listen(SERVER_SOCKET_NAME) != true) {
        if (m_server->serverError() == QAbstractSocket::AddressInUseError) {
            if (m_server->removeServer(SERVER_SOCKET_NAME) != true) {
                qCritical() << "Can't remove old file socket" << SERVER_SOCKET_NAME;
                qCritical() << "Error:" << m_server->errorString();
                Q_ASSERT(false);
                return false;
            }
            m_server->listen(SERVER_SOCKET_NAME);
        } else {
            qCritical() << "Can't create pipe" << SERVER_SOCKET_NAME << "for communicate with plugin";
            qCritical() << "Error:" << m_server->errorString();
        }
    }

    if (!m_server->isListening()) {
        qCritical() << "External plugin server isn't listening.";
        return false;
    }

    connect(m_server, SIGNAL(newConnection()), this, SLOT(storeNewConnection()));

    return true;
}

void ExternalPluginServer::storeNewConnection()
{
    qDebug() << "Plugin is requesting connection";
    QLocalSocket *connection = m_server->nextPendingConnection();
    if (connection == NULL) {
        qCritical() << "Mystery plugin is requesting connection, but escapes from connect";
        Q_ASSERT(false);
    }
    bool res;
    res = connect(connection, SIGNAL(readyRead()), this, SLOT(readPluginData()));
    Q_ASSERT(res == true);
    res = connect(connection, SIGNAL(disconnected()), this, SLOT(forgetConnection()));
    Q_ASSERT(res == true);
    m_pluginConnections.insertMulti("-1", connection);
}

/**
 * @brief read data from plugin. Divide it on packets and handle packets by its type.
 */
void ExternalPluginServer::readPluginData()
{
    QLocalSocket *connection = qobject_cast<QLocalSocket *>(QObject::sender());
    if (connection == 0) {
        qWarning() << "Unknown type of sender";
        return;
    }
    QByteArray data = connection->readAll();
    QList<QByteArray> packets;

    //  Divide readed data on packets by separator - PLUGIN_PACKET_PREAMBLE
    int receivedBytes = data.size();
    int pos = data.indexOf(PLUGIN_PACKET_PREAMBLE);
    while (pos >= 0) {
        if ((receivedBytes - pos) < (int)PLUGIN_PACKET_LENGTH) {
            qWarning() << "Some data from plugin is missed. Received:" << receivedBytes;
            qCritical() << "Recieved from plugin:" << data;
            Q_ASSERT(false);
            break;
        }
        QByteArray packet;
        packet = data.mid(pos, (int)PLUGIN_PACKET_LENGTH);
        packets.append(packet);
        pos = data.indexOf(PLUGIN_PACKET_PREAMBLE, pos + (int)PLUGIN_PACKET_LENGTH);
    }

    //  Handle plugin data
    foreach (QByteArray packet, packets) {
        if (packet.size() != (int)PLUGIN_PACKET_LENGTH) {
            qCritical() << "Incorrect packet size:" << packet.size();
            qCritical() << "Recieved from plugin:" << data;
            Q_ASSERT(false);
            continue;
        }

        pluginPacket_s *data = (pluginPacket_s *)packet.data();
        qDebug() << "UID:" << data->uid << "Events:" << data->eventsCount;
        QString uid = QString("%1").arg(data->uid);

        //  Verify and bind unique connection. Then send events notification to PluginManager
        if (m_pluginConnections.contains(uid)) {
            if (m_pluginConnections.value(uid) != connection) {
                qCritical() << "Simultaneous access from different plugins by same UID:" << uid;
                continue;
            }
            emit eventsGot(data->uid, data->eventsCount);
        } else {
            QList<QLocalSocket *> unbindedConnections = m_pluginConnections.values("-1");
            if (unbindedConnections.contains(connection)) {
                m_pluginConnections.remove("-1", connection);
                if (m_pluginConnections.contains(uid)) {
                    qCritical() << "Plugins with same UID:" << uid << "has been already exist";
                    continue;
                }
                qDebug() << "Bind plugin with UID:" << uid;
                m_pluginConnections.insert(uid, connection);
                emit eventsGot(data->uid, data->eventsCount);
            } else {
                qCritical() << "Recieved data from plugin, which doesn't register in unbinded connections. UID:" << uid;
                continue;
            }
        }
    }
}

void ExternalPluginServer::forgetConnection()
{
    QLocalSocket *connection = qobject_cast<QLocalSocket *>(QObject::sender());
    if (connection == 0) {
        qWarning() << "Unknown type of sender";
        return;
    }
    QString uid = m_pluginConnections.key(connection, "");
    if (uid.isEmpty()) {
        qCritical() << "Can't find corresponding UID for disconnected plugin";
        return;
    }
    qDebug() << "Plugin with UID" << uid << "is disconnected";
    m_pluginConnections.remove(uid, connection);
}
