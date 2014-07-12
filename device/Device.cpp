#include <QDebug>
#include "Device.h"

Device::Device(QObject *parent) :
    QObject(parent)
{
}

void Device::setLedMode(qint32 pluginUid, LedMode_e mode)
{
    qDebug() << "Set led mode" << mode << "for plugin" << pluginUid;
    emit buttonPressed(pluginUid);
}
