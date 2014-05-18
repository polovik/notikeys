#ifndef EXTERNALPLUGINPROTOCOL_H
#define EXTERNALPLUGINPROTOCOL_H

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

static const char *SERVER_SOCKET_NAME = "NOTIKEYS_PLUGINS_SOCKET";
static const quint8 PLUGIN_PACKET_PREAMBLE = 0xEF;

typedef struct {
    quint8 preamble;
    qint32 uid;
    qint32 eventsCount;
} pluginPacket_s;

#define PLUGIN_PACKET_LENGTH sizeof(pluginPacket_s)

#pragma pack(pop)   /* restore original alignment from stack */

#endif // EXTERNALPLUGINPROTOCOL_H
