#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>

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
    void requestConnect();
    void requestStartCalibration();
    void requestStopCalibration();
    void requestProgramming();
    void requestStartTesting();
    void requestStopTesting();
    void requestFlashing();
    void requestReset();

signals:
    void SIG_DEVICE_OPENED();
    void SIG_DEVICE_CLOSED();
    void SIG_HANDSHAKED();
    void SIG_CONNECTED();
    void SIG_CALIBRATION_INFO();
    void SIG_CALIBRATION_STOPPED();
    void SIG_SETTINGS_WRITTEN();
    void SIG_TESTING_STOPPED();
    void SIG_FLASHING_FINISHED();

private slots:
    void parsePacket(const QByteArray &rawData);

private:
    QString m_readBuffer;
    UartPort* m_uartPort;
};

#endif // DEVICE_H
