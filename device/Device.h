#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QTimer>
#include "protocol_pc_to_device.h"

class UartPort;

class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = 0);
    ~Device();
    bool init();
    void close();

public slots:
    void openDevice();
    void closeDevice();
    void requestHandshake();
    void requestLedCotrol(quint8 pos, quint16 state);
    void requestReset();

signals:
    void SIG_DEVICE_OPENED();
    void SIG_DEVICE_CLOSED();
    void SIG_HANDSHAKED();
    void SIG_DEVICE_NOT_ANSWER();

private slots:
    void parseData(const QByteArray &rawData);

private:
    QByteArray assemblyPacket(packet_type_e command, const packet_data_u *data, unsigned int dataLength = 0);
    void rewindIncomingBuffer();
    bool parsePacket();

    QByteArray m_readBuffer;
    UartPort* m_uartPort;
    QTimer m_deviceAnswerTimer;
};

#endif // DEVICE_H
