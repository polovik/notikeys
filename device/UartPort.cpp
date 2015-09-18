#include <QDebug>
#include <QByteArray>
#include <QString>
#include <QSerialPort>
#include "UartPort.h"

UartPort::UartPort(configuration_e deviceType, QObject *parent) :
    m_serialPort(NULL)
{
    Q_UNUSED(parent);
    needClosePort = false;
    m_deviceType = deviceType;
}

UartPort::~UartPort()
{
    close();
}

/** @brief For ensure that device has been opened - wait signal portOpened
 */
void UartPort::open(const QString &serialPortName)
{
    QMutexLocker locker(&portMutex);
    QMutexLocker lockerTransmit(&transmitMutex);
    m_portName = serialPortName;
    m_transmitBuffer.clear();
}

/** @brief For ensure that device has been closed - wait signal portClosed
 */
void UartPort::close()
{
    QMutexLocker locker(&portMutex);
    needClosePort = true;
}

/** @brief Before sending packet, you should ensure that you have received signal portOpened(true).
 */
void UartPort::sendPacket(const QByteArray& packet)
{
    QMutexLocker lockerPort(&portMutex);
    QMutexLocker lockerTransmit(&transmitMutex);
    if (m_portName.isEmpty()) {
        qCritical() << "Can't send packet: COM port has been closed";
        return;
    }
    m_transmitBuffer.append(packet);
}

bool UartPort::configurePort()
{
    if (m_serialPort == NULL) {
        qCritical("Serial port hasn't been created");
        return false;
    }
    if (m_serialPort->portName().isEmpty()) {
        qCritical("Serial port's name is empty");
        return false;
    }
    if (!m_serialPort->open(QIODevice::ReadWrite/* | QIODevice::Unbuffered*/)) {
        qCritical() << "Can't open serial port " << m_serialPort->portName();
        return false;
    }
    QSerialPort::BaudRate baudRate = QSerialPort::UnknownBaud;
    QSerialPort::Parity parity = QSerialPort::UnknownParity;
    if (m_deviceType == CONFIGURATION_CPRO41W) {
        m_textProtocol = true;
        m_uartTimeoutMS = 10;
        baudRate = QSerialPort::Baud115200;
        parity = QSerialPort::NoParity;
    } else if (m_deviceType == CONFIGURATION_CS04M2) {
        m_textProtocol = true;
        m_uartTimeoutMS = 100;
        baudRate = QSerialPort::Baud9600;
        parity = QSerialPort::NoParity;
    }
    if (!m_serialPort->setBaudRate(baudRate)) {
        qCritical("Can't set baudrate");
        return false;
    }
    if (!m_serialPort->setDataBits(QSerialPort::Data8)) {
        qCritical("Can't set databits");
        return false;
    }
    if (!m_serialPort->setStopBits(QSerialPort::OneStop)) {
        qCritical("Can't set stopbits");
        return false;
    }
    if (!m_serialPort->setParity(parity)) {
        qCritical("Can't set parity");
        return false;
    }
    if (!m_serialPort->setFlowControl(QSerialPort::NoFlowControl)) {
        qCritical("Can't set flowcontrol");
        return false;
    }
    m_serialPort->setReadBufferSize(512);

    if (!m_serialPort->clear()) {
        qCritical("Can't clear UART buffers");
        return false;
    }

    qDebug() << "Port" << m_serialPort->portName() << "has been successfully opened";
    return true;
}

void UartPort::run()
{
    bool needSendData = false;

    m_serialPort = new QSerialPort("");
    forever {
        //  Close serial port
        portMutex.lock();
        if (needClosePort == true) {
            m_transmitBuffer.clear();
            qDebug() << "Close port" << m_serialPort->portName();
            m_portName.clear();
            if (!m_serialPort->portName().isEmpty()) {
                m_serialPort->close();
                m_serialPort->setPortName(m_portName);
            }
            emit portClosed();
        }
        needClosePort = false;
        portMutex.unlock();

        //  Sleep if device doesn't connected
        if (m_portName.isEmpty()) {
            msleep(100);
            continue;
        }

        //  Open serial port
        portMutex.lock();
        if (m_serialPort->portName() != m_portName) {
            if (!m_serialPort->portName().isEmpty()) {
                qCritical() << "Try open port" << m_portName << "without closing previous" << m_serialPort->portName();
                Q_ASSERT(false);
                m_portName.clear();
                portMutex.unlock();
                continue;
            }
            qDebug() << "Try open port" << m_portName;
            //  do not clear m_transmitBuffer, because reopening may match with sending packet
            m_serialPort->setPortName(m_portName);
            bool configured = configurePort();
            if (!configured) {
                m_portName.clear();
                m_serialPort->close();
                m_serialPort->setPortName(m_portName);
            }
            portMutex.unlock();
            emit portOpened(configured);
        } else {
            portMutex.unlock();
        }

        /*  Send data to device */
        transmitMutex.lock();
        needSendData = !m_transmitBuffer.isEmpty();
        transmitMutex.unlock();

        if (needSendData) {
            transmitMutex.lock();
            if (m_serialPort->write(m_transmitBuffer.data(), m_transmitBuffer.length()) != m_transmitBuffer.length()) {
                qCritical("Can't write data to serial port");
                transmitMutex.unlock();
                continue;
            }
//            if (m_textProtocol)
//                qDebug() << "SENT:" << m_transmitBuffer;
//            else
//                qDebug("SENT: 0x%s", qPrintable(m_transmitBuffer.toHex().toUpper()));
            m_transmitBuffer.clear();
            transmitMutex.unlock();

            if (!m_serialPort->waitForBytesWritten(m_uartTimeoutMS)) {
                if (m_deviceType != CONFIGURATION_CS04M2) {
                    qCritical("Writing is timed out");
                    continue;
                }
            }
        }

        /*  Read data from device   */
        if (!m_serialPort->waitForReadyRead(m_uartTimeoutMS))
            continue;
        QByteArray input = m_serialPort->readAll();
        if (input.size() < 1)
            continue;
//        if (m_textProtocol)
//            qDebug() << "RECEIVED:" << input;
//        else
//            qDebug("RECEIVED: 0x%s", qPrintable(input.toHex().toUpper()));
        emit dataReceived(input);
    }
}
