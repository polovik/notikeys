#ifndef UARTPORT_H
#define UARTPORT_H

#include <QObject>
#include <QThread>
#include <QMutex>

class QSerialPort;

typedef enum {
    CONFIGURATION_UNKNOWN  = 0,
    CONFIGURATION_CPRO41W  = 1,
    CONFIGURATION_CS04M2   = 2
} configuration_e;

class UartPort : public QThread
{
    Q_OBJECT
public:
    explicit UartPort(configuration_e deviceType, QObject *parent = 0);
    ~UartPort();

public slots:
    void open(const QString &serialPortName);
    void sendPacket(const QByteArray &packet);
    void close();

protected:
    void run() Q_DECL_OVERRIDE;

private:
    bool configurePort();

    configuration_e m_deviceType;
    int m_uartTimeoutMS;
    bool m_textProtocol;
    QSerialPort *m_serialPort;
    QByteArray m_transmitBuffer;
    QString m_portName;
    QMutex portMutex;
    QMutex transmitMutex;
    bool needClosePort;

signals:
    void dataReceived(const QByteArray &packet);
    void portOpened(bool success);
    void portClosed();
};

#endif // UARTPORT_H
