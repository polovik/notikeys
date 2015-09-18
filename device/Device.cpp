#include <QDebug>
#include <QSerialPortInfo>
#include "Device.h"
#include "UartPort.h"

namespace NS_Device {
    quint16 DEVICE_USB_VID = 0x04D8;
    quint16 DEVICE_USB_PID = 0x000A;
}

Device::Device(QObject *parent) :
    QObject(parent), m_uartPort(NULL)
{
    m_uartPort = new UartPort(CONFIGURATION_SHUTTER);
    m_uartPort->start();

    connect(m_uartPort, SIGNAL(dataReceived(const QByteArray &)), this, SLOT(parsePacket(const QByteArray &)));
    connect(m_uartPort, SIGNAL(portOpened()), SIGNAL(SIG_DEVICE_OPENED()));
    connect(m_uartPort, SIGNAL(portClosed()), SIGNAL(SIG_DEVICE_CLOSED()));
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
            qDebug() << "Shutter's control device is found on" << portName;
            break;
        }
    }

    if (portName.isEmpty()) {
        qWarning() << "Shutter's control device is disconnected";
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

void Device::parsePacket(const QByteArray &rawData)
{
    QString data(rawData);
    if (data.isEmpty())
        return;
    m_readBuffer.append(rawData);
    qDebug() << "Parse packet:" << m_readBuffer;

    if (m_readBuffer.contains("222")) {
        m_readBuffer.clear();
        emit SIG_HANDSHAKED();
    } else if (m_readBuffer.contains("444")) {
        m_readBuffer.clear();
        emit SIG_CONNECTED();
    } else if (m_readBuffer.contains("666")) {
        m_readBuffer.clear();
        emit SIG_CALIBRATION_INFO();
    } else if (m_readBuffer.contains("671")) {
        m_readBuffer.clear();
        emit SIG_CALIBRATION_STOPPED();
    } else if (m_readBuffer.contains("888")) {
        m_readBuffer.clear();
        emit SIG_SETTINGS_WRITTEN();
    } else if (m_readBuffer.contains(":31")) {
        m_readBuffer.clear();
        emit SIG_TESTING_STOPPED();
    } else if (m_readBuffer.contains("2345")) {
        m_readBuffer.clear();
        emit SIG_FLASHING_FINISHED();
    } else {
        qWarning() << "Unknown answer:" << data;
    }
}

void Device::requestHandshake()
{
    QString command = QString("111");
    m_uartPort->sendPacket(command.toLatin1());
}

void Device::requestConnect()
{
    QString command = QString("333");
    m_uartPort->sendPacket(command.toLatin1());
}

void Device::requestStartCalibration()
{
    QString command = QString("555");
    m_uartPort->sendPacket(command.toLatin1());
}

void Device::requestStopCalibration()
{
    QString command = QString("560");
    m_uartPort->sendPacket(command.toLatin1());
}

void Device::requestProgramming()
{
    QString command = QString("777");
    m_uartPort->sendPacket(command.toLatin1());
}

void Device::requestStartTesting()
{
    QString command = QString("900");
    m_uartPort->sendPacket(command.toLatin1());
}

void Device::requestStopTesting()
{
    QString command = QString("920");
    m_uartPort->sendPacket(command.toLatin1());
}

void Device::requestFlashing()
{
    QString command = QString("1234");
    m_uartPort->sendPacket(command.toLatin1());
}

void Device::requestReset()
{
    QString command = QString("reset");
    m_uartPort->sendPacket(command.toLatin1());
}

