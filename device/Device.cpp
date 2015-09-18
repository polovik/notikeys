#include <QDebug>
//#include <QTest>
#include <QSerialPortInfo>
#include "Device.h"
#include "UartPort.h"
#include "../gui/PluginInterface.h"

Device::Device(QObject *parent) :
    QObject(parent), m_uartPort(NULL)
{
    m_uartPort = new UartPort(CONFIGURATION_CS04M2);
    m_uartPort->start();

    connect(m_uartPort, SIGNAL(dataReceived(const QByteArray &)), this, SLOT(parsePacket(const QByteArray &)));

    m_responseTimer.setSingleShot(true);
    connect(&m_responseTimer, SIGNAL(timeout()), this, SLOT(handleResponseTimeout()));
}

Device::~Device()
{
    close();
}

bool Device::init()
{
    static bool inited = false;
    static QString portName;

    connect(m_uartPort, &UartPort::portOpened, [this](bool success) {
        if (success) {
            qDebug() << "SwitchCS04M::init portOpened";
            QString command = QString("ACTIVATE\r\n");
            m_uartPort->sendPacket(command.toLatin1());
        } else {
            inited = false;
        }
    });
    QList<QSerialPortInfo> portsInfo = QSerialPortInfo::availablePorts();
    foreach (const QSerialPortInfo &portInfo, portsInfo) {
        qDebug() << portInfo.portName() << portInfo.vendorIdentifier() << portInfo.productIdentifier()
                 << portInfo.description() << portInfo.manufacturer() << portInfo.serialNumber();
    }

//    if ((portInfo.vendorIdentifier() == NS_MODEM_3G::huaweiE150vendorID) &&
//        (portInfo.productIdentifier() == NS_MODEM_3G::huaweiE150productID)) {
//        qDebug() << "Huawei E150 has been detected on port" << m_portName;
//        if (portInfo.description().contains(NS_MODEM_3G::huaweiE150description, Qt::CaseSensitive)) {

//    m_uartPort->open(portName);

//    QTest::qWait(3000);
//    this->disconnect(); //  Disconnect all slots from own signals - prevent double calling

    if (inited)
        qDebug() << "Keyboard is successfully inited";

    return inited;
}

void Device::close()
{
    if (m_uartPort == NULL) {
        qWarning() << "Serial port is closed, but should be opened";
        return;
    }
    m_uartPort->close();
}

void Device::parsePacket(const QByteArray &rawData)
{
    QString data(rawData);
    data = data.trimmed();
    if (data.isEmpty())
        return;
    qDebug() << "Parse packet:" << rawData;

//    m_readBuffer.append(rawData);
//    if (m_readBuffer.contains("OK!")) {
//        m_readBuffer.clear();
//        emit commandApplied();
//    } else {
//        qWarning() << "Unknown answer:" << data;
//    }
}

void Device::handleResponseTimeout()
{
    qCritical() << "Keyboard doesn't response on time";
}

void Device::setLedMode(QString pluginUid, LedMode_e mode)
{
    qDebug() << "Set led mode" << QString::number(mode) << "for plugin" << pluginUid;
    emit buttonPressed(pluginUid);
}
