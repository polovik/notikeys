#include <QDebug>
#include <QSerialPortInfo>
#include "Device.h"
#include "UartPort.h"
extern "C" {
    #include "crc16.h"
}

namespace NS_Device {
    quint16 DEVICE_USB_VID = 0x04D8;
    quint16 DEVICE_USB_PID = 0x000A;
}

Device::Device(QObject *parent) :
    QObject(parent), m_uartPort(NULL)
{
    m_uartPort = new UartPort(CONFIGURATION_KEYBOARD);
    m_uartPort->start();

    connect(m_uartPort, SIGNAL(dataReceived(const QByteArray &)), this, SLOT(parsePacket(const QByteArray &)));
    connect(m_uartPort, SIGNAL(portOpened()), SIGNAL(SIG_DEVICE_OPENED()));
    connect(m_uartPort, SIGNAL(portClosed()), SIGNAL(SIG_DEVICE_CLOSED()));
    connect(&m_deviceAnswerTimer, SIGNAL(timeout()), SIGNAL(SIG_DEVICE_NOT_ANSWER()));
    m_deviceAnswerTimer.setSingleShot(true);
    m_deviceAnswerTimer.setInterval(1000);
}

Device::~Device()
{
//    closeDevice();
}

void Device::openDevice()
{
    QString portName;

    QList<QSerialPortInfo> portsInfo = QSerialPortInfo::availablePorts();
    foreach (const QSerialPortInfo &portInfo, portsInfo) {
        qDebug() << portInfo.portName() << portInfo.vendorIdentifier() << portInfo.productIdentifier()
                 << portInfo.description() << portInfo.manufacturer() << portInfo.serialNumber();
        if ((portInfo.vendorIdentifier() == NS_Device::DEVICE_USB_VID)
            && (portInfo.productIdentifier() == NS_Device::DEVICE_USB_PID)) {
            portName = portInfo.portName();
            qDebug() << "Keyboard is found on" << portName;
            break;
        }
    }

    if (portName.isEmpty()) {
        qWarning() << "Keyboard is disconnected";
        emit SIG_DEVICE_CLOSED();
        return;
    }

    m_uartPort->open(portName);
}

void Device::closeDevice()
{
    if (m_uartPort == NULL) {
        qWarning() << "Serial port has been already closed";
        return;
    }
    m_uartPort->close();
}


QByteArray Device::assemblyPacket(packet_type_e command, const packet_data_u *data, unsigned int dataLength)
{
    //  Fill header
    packet_header header;
    memset(&header, 0x00, HEADER_LENGTH);

    header.preamble = PREAMBLE;
    header.direction = DIRECTION_TO_DEVICE;
    header.encrypted = ENCRYPTION_OFF;
    header.type = command;
    header.reserved = 0xFFFFFFFF;

    //  Fill buffer for data
    char *dataField = NULL;
    unsigned int dataFieldSize = 0;

    switch (command) {
    case GET_DEVICE_ID:
    case GET_STATUS:
    case GET_BUTTONS_STATE:
        break;
    case SET_LEDS_STATE:
        dataFieldSize = 3 * sizeof(button_state_s);
        dataField = (char *)malloc(dataFieldSize);
        memcpy(dataField, &data->buttons_state, dataFieldSize);
        break;
    default:
        qCritical() << "Unknown command:" << command;
        Q_ASSERT(false);
        if (data != NULL) {
            dataFieldSize = dataLength;
            dataField = (char *)malloc(dataFieldSize);
            memcpy(dataField, data, dataFieldSize);
        }
    }

    header.length = 6 + dataFieldSize;

    //  Join header with data
    char *packet = (char *)malloc(HEADER_LENGTH + dataFieldSize + CRC_LEN);
    memcpy(packet, &header, HEADER_LENGTH);
    if ((dataField != NULL) && (dataFieldSize > 0)) {
        memcpy(packet + HEADER_LENGTH, dataField, dataFieldSize);
        free(dataField);
    }

    //  Calculate CRC over: length, encrypted, type, reserved, data
    quint16 crc16 = crc16_ccitt((uint8_t *)packet + 2, header.length + 2);

    //  Join crc to packet
    memcpy(packet + HEADER_LENGTH + dataFieldSize, &crc16, CRC_LEN);
    QByteArray assembliedPacket(packet, HEADER_LENGTH + dataFieldSize + CRC_LEN);
    free(packet);

    qDebug("GENERATED PACKET: 0x%s", qPrintable(assembliedPacket.toHex().toUpper()));
    return assembliedPacket;
}

void Device::rewindIncomingBuffer()
{
    //  Possible content of m_readBuffer buffer:
    //  7F.......
    //  ....7F...
    //  .......7F
    //  7F...7F..
    //  7F.....7F
    //  ..7F...7F
    int startPos = m_readBuffer.indexOf(PREAMBLE);
    if (startPos == 0)
        startPos = m_readBuffer.indexOf(PREAMBLE, 1);

    if (startPos == -1)
        m_readBuffer.clear();
    else
        m_readBuffer.remove(0, startPos);

    if ((unsigned int)m_readBuffer.size() > (HEADER_LENGTH + CRC_LEN)) {
        QByteArray stub;
        parsePacket(stub);
    }
}

void Device::parsePacket(const QByteArray& raw_data)
{
    extern bool g_verboseOutput;

    //  Add new data in common buffer
    m_readBuffer.append(raw_data);
    qDebug("parse: %d, 0x%s", m_readBuffer.size(), qPrintable(m_readBuffer.toHex().toUpper()));

    //  Verify prefix of packet
    if (m_readBuffer.size() < 4) {
        qDebug() << "Packet hasn't been got yet. Expect major 4 bytes. Got:" << m_readBuffer.size() << "bytes";
        return;
    }
    packet_header *header = (packet_header *)m_readBuffer.data();
    if ((header->preamble != PREAMBLE) || (header->direction != DIRECTION_FROM_DEVICE)) {
        qCritical("Incorrect prefix of packet: 0x%X 0x%X", header->preamble, header->direction);
        rewindIncomingBuffer();
//        Q_ASSERT(false);
        return;
    }

    //  Wait until get full packet
    unsigned int dataSize = header->length;
    if (dataSize > MAX_DATA_SIZE) {
        qCritical("Incorrect size of data section: %dbytes. Allowed: %dbytes", dataSize, MAX_DATA_SIZE);
        rewindIncomingBuffer();
        return;
    }
    if ((unsigned int)m_readBuffer.size() < (4 + dataSize + CRC_LEN)) {
        if (g_verboseOutput)
            qDebug() << "Packet hasn't been got yet. Expect:" << (4 + dataSize + CRC_LEN) << "bytes, got:" << m_readBuffer.size() << "bytes";
        return;
    }
    if (header->encrypted != ENCRYPTION_OFF) {
        qCritical() << "Encrypted packets isn't supported yet";
        rewindIncomingBuffer();
        Q_ASSERT(false);
        return;
    }

    //  Verify integrity of data field
    quint16 calculatedCRC = crc16_ccitt((uint8_t *)m_readBuffer.data() + 2, dataSize + 2);
    quint16 packetCRC;
    memcpy(&packetCRC, m_readBuffer.data() + 4 + dataSize, CRC_LEN);
    if (calculatedCRC != packetCRC) {
        qCritical("Incorrect CRC = 0x%04X, should be 0x%04X", calculatedCRC, packetCRC);
        rewindIncomingBuffer();
        Q_ASSERT(false);
        return;
    }

    //  Extract meaningful data
    packet_type_e command = (packet_type_e)header->type;
    packet_data_u *data = (packet_data_u *)(m_readBuffer.data() + HEADER_LENGTH);

    switch (command) {
    case GET_DEVICE_ID: {
        m_deviceAnswerTimer.stop();
        QString id = QString::fromLatin1((char *)data->device_id, 8);
        QString name = id.section(" ", 0, 0);
        QString ver = id.section(" ", 1, 1);
        ver.insert(2, ".");
        ver.insert(1, ".");
        qDebug() << "Device ID =" << name << "version =" << ver;
        if (id.startsWith("Keys"))
            emit SIG_HANDSHAKED();
        break;
    }
    case GET_STATUS: {
        quint16 errors = data->device_status.errors;
        quint32 rtc = data->device_status.rtc;
        qDebug("Device status: errors 0x%04X, rtc %d",
               errors, rtc);
        break;
    }
    default:
        qWarning("Unknown command id %d", command);
        Q_ASSERT(false);
        break;
    }
//    qDebug("rest: %d, 0x%s", dataSize, qPrintable(m_readBuffer.toHex().toUpper()));
    rewindIncomingBuffer();
}

void Device::requestHandshake()
{
    QByteArray packet = assemblyPacket(GET_DEVICE_ID, NULL);
    m_deviceAnswerTimer.start();
    m_uartPort->sendPacket(packet);
}

void Device::requestReset()
{
//    QString command = QString("reset");
//    m_uartPort->sendPacket(command.toLatin1());
}

