#ifndef UARTPORT_H
#define UARTPORT_H

#include <QObject>
#include <QThread>
#include <QMutex>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <WinBase.h>
#endif

class QSerialPort;

typedef enum {
    CONFIGURATION_UNKNOWN  = 0,
    CONFIGURATION_KEYBOARD = 1
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

signals:
    void dataReceived(const QByteArray &packet);
    void portOpened();
    void portClosed();

protected:
    void run() Q_DECL_OVERRIDE;

private:
    bool openPort();
    bool configurePort();
    bool sendData();
    QByteArray receiveData();
    void closePort();

    configuration_e m_deviceType;
    int m_uartTimeoutMS;
    bool m_textProtocol;

#ifdef Q_OS_WIN
    HANDLE m_serialPort;
#else
    QSerialPort *m_serialPort;
#endif

    QByteArray m_transmitBuffer;
    QString m_portName;
    QMutex portMutex;
    QMutex transmitMutex;
    bool m_needClosePort;
};

#endif // UARTPORT_H
