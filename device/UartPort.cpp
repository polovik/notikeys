#include <QDebug>
#include <QByteArray>
#include <QString>
#ifndef Q_OS_WIN
    #include <QSerialPort>
#endif
#include "UartPort.h"

UartPort::UartPort(configuration_e deviceType, QObject *parent)
{
    Q_UNUSED(parent);
    m_needClosePort = false;
    m_deviceType = deviceType;

#ifdef Q_OS_WIN
    m_serialPort = INVALID_HANDLE_VALUE;
#else
    m_serialPort = NULL;
#endif
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
    m_needClosePort = true;
}

/** @brief Before sending packet, you should ensure that you have received signal portOpened().
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

#ifdef Q_OS_WIN
bool UartPort::openPort()
{
    qDebug() << "Try open port" << m_portName;
    wchar_t portName[30];
    memset(portName, 0x00, sizeof(portName));
    m_portName.toWCharArray(portName);
    m_serialPort = CreateFile(portName,                     // lpFileName
                              GENERIC_READ | GENERIC_WRITE, // dwDesiredAccess
                              0,                            // dwShareMode
                              NULL,                         // lpSecurityAttributes
                              OPEN_EXISTING,                // dwCreationDisposition
                              FILE_ATTRIBUTE_NORMAL,        // dwFlagsAndAttributes
                              NULL);                        // hTemplateFile
    if (m_serialPort == INVALID_HANDLE_VALUE) {
        qCritical() << "Can't open port" << m_portName << "Error:" << QString::number(GetLastError());
        needClosePort = true;
        portMutex.unlock();
        emit portClosed();
        continue;
    }

    //  Connection setting up
    DCB dcbSerialParams;
    memset(&dcbSerialParams, 0x00, sizeof(dcbSerialParams));
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(m_serialPort, &dcbSerialParams) == FALSE) {
        qCritical() << "Can't get port's state. Error:" << QString::number(GetLastError());
        needClosePort = true;
        portMutex.unlock();
        emit portClosed();
        continue;
    }
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (SetCommState(m_serialPort, &dcbSerialParams) == FALSE) {
        qCritical() << "Can't configure port. Error:" << QString::number(GetLastError());
        needClosePort = true;
        portMutex.unlock();
        emit portClosed();
        continue;
    }

    COMMTIMEOUTS timeouts;
    memset(&timeouts, 0x00, sizeof(timeouts));
    timeouts.ReadIntervalTimeout = 5;
    timeouts.ReadTotalTimeoutMultiplier = 5;
    timeouts.ReadTotalTimeoutConstant = 10;
    timeouts.WriteTotalTimeoutMultiplier = 5;
    timeouts.WriteTotalTimeoutConstant = 5;
    if (SetCommTimeouts(m_serialPort, &timeouts) == FALSE) {
        qCritical() << "Can't set timeouts for port. Error:" << QString::number(GetLastError());
        needClosePort = true;
        portMutex.unlock();
        emit portClosed();
        continue;
    }
    emit portOpened();
}

bool UartPort::configurePort()
{
}

bool UartPort::sendData()
{
    DWORD bufferSize = m_transmitBuffer.length();
    DWORD bytesWritten = 0;
    BOOL written = WriteFile(m_serialPort, m_transmitBuffer.data(), bufferSize, &bytesWritten, NULL);
    if ((written == FALSE) || (bytesWritten != bufferSize)) {
        qCritical() << "Can't write data to serial port. Error:" << QString::number(GetLastError())
                    << "Written:" << QString::number(bytesWritten) << "Sent:" << QString::number(bufferSize);
        return false;
    }

    return true;
}

int UartPort::receiveData()
{
    char rxBuffer[100];
    int received;
    DWORD readBytes = 0;
    QByteArray rxData;

    BOOL readResult = ReadFile(m_serialPort, rxBuffer, sizeof(rxBuffer), &readBytes, NULL);
    if (readResult == FALSE) {
        qCritical() << "Can't read data from serial port. Error:" << QString::number(GetLastError());
        return rxData;
    }

    if (recieved < 1)
        return rxData;

    rxData = QByteArray::fromRawData(rxBuffer, recieved);
    return rxData;
}

void UartPort::closePort()
{
    if (m_serialPort == INVALID_HANDLE_VALUE) {
        qCritical() << "Try to close closed port";
        Q_ASSERT(false);
        return;
    }

    if (CloseHandle(m_serialPort) == FALSE)
        qCritical() << "Can't close port" << m_portName << "Error:" << QString::number(GetLastError());
    m_serialPort = INVALID_HANDLE_VALUE;
}

#else

bool UartPort::openPort()
{
    if (!m_serialPort->portName().isEmpty()) {
        qCritical() << "Try open port" << m_portName << "without closing previous" << m_serialPort->portName();
        Q_ASSERT(false);
        m_portName.clear();
        return false;
    }

    qDebug() << "Try open port" << m_portName;

    //  do not clear m_transmitBuffer, because reopening may match with sending packet
    m_serialPort->setPortName(m_portName);
    bool configured = configurePort();
    if (configured) {
        return true;
    } else {
        m_portName.clear();
        m_serialPort->close();
        m_serialPort->setPortName(m_portName);
        return false;
    }
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
    if (m_deviceType == CONFIGURATION_KEYBOARD) {
        m_textProtocol = false;
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

bool UartPort::sendData()
{
    if (m_serialPort->write(m_transmitBuffer.data(), m_transmitBuffer.length()) != m_transmitBuffer.length()) {
        qCritical("Can't write data to serial port");
        transmitMutex.unlock();
        return false;
    }

    if (!m_serialPort->waitForBytesWritten(m_uartTimeoutMS)) {
        qCritical("Writing is timed out");
        return false;
    }

    return true;
}

QByteArray UartPort::receiveData()
{
    QByteArray rxData;

    if (!m_serialPort->waitForReadyRead(m_uartTimeoutMS))
        return rxData;

    rxData = m_serialPort->readAll();
    return rxData;
}

void UartPort::closePort()
{
    if (m_serialPort->portName().isEmpty()) {
        qCritical() << "Try to close closed port";
        Q_ASSERT(false);
        return;
    }

    m_serialPort->close();
    m_serialPort->setPortName("");
}
#endif

void UartPort::run()
{
    bool needSendData = false;
    m_needClosePort = false;

#ifndef Q_OS_WIN
    m_serialPort = new QSerialPort("");
#endif

    forever {
        //  Close serial port
        portMutex.lock();
        if (m_needClosePort == true) {
            m_transmitBuffer.clear();
            qDebug() << "Close port" << m_portName;
            closePort();
            m_portName.clear();
            m_needClosePort = false;
            portMutex.unlock();
            emit portClosed();
            continue;
        }
        portMutex.unlock();

        //  Sleep if device doesn't connected
        if (m_portName.isEmpty()) {
            msleep(100);
            continue;
        }

        msleep(10);

        //  Open serial port
        portMutex.lock();
#ifdef Q_OS_WIN
        if (m_serialPort == INVALID_HANDLE_VALUE) {
#else
        if (m_serialPort->portName() != m_portName) {
#endif
            bool opened = openPort();

            portMutex.unlock();
            if (opened) {
                emit portOpened();
            } else {
                emit portClosed();
                continue;
            }
        } else {
            portMutex.unlock();
        }

        /*  Send data to device */
        transmitMutex.lock();
        needSendData = !m_transmitBuffer.isEmpty();
        transmitMutex.unlock();

        if (needSendData) {
            transmitMutex.lock();
            bool sent = sendData();
            if (!sent) {
                transmitMutex.unlock();
                close();
                continue;
            }

            if (m_textProtocol)
                qDebug() << "SENT:" << m_transmitBuffer;
            else
                qDebug("SENT: 0x%s", qPrintable(m_transmitBuffer.toHex().toUpper()));
            m_transmitBuffer.clear();
            transmitMutex.unlock();
        }

        /*  Read data from device   */
        QByteArray input = receiveData();
        if (input.size() < 1)
            continue;

        if (m_textProtocol)
            qDebug() << "RECEIVED:" << input;
        else
            qDebug("RECEIVED: 0x%s", qPrintable(input.toHex().toUpper()));
        emit dataReceived(input);
    }
}
