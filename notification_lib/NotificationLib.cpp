#include <QDebug>
#include <QLocalSocket>
#include "NotificationLib.h"
#include "../gui/ExternalPluginProtocol.h"

void notify(int pluginUID, int eventsCount)
{
    QLocalSocket socket;
    socket.connectToServer(SERVER_SOCKET_NAME);
    socket.waitForConnected(1000);

    pluginPacket_s packet;
    packet.preamble = PLUGIN_PACKET_PREAMBLE;
    packet.uid = pluginUID;
    packet.eventsCount = eventsCount;
    socket.write((char *)&packet, PLUGIN_PACKET_LENGTH);
    socket.waitForBytesWritten(1000);

    socket.disconnectFromServer();
    if (socket.state() != QLocalSocket::UnconnectedState) {
        socket.waitForDisconnected(1000);
    }
}
