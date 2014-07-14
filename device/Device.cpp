#include <QDebug>
#include "Device.h"
#include "../gui/PluginInterface.h"

Device::Device(QObject *parent) :
    QObject(parent)
{
}

void Device::setLedMode(QString pluginUid, LedMode_e mode)
{
    qDebug() << "Set led mode" << QString::number(mode) << "for plugin" << pluginUid;
    emit buttonPressed(pluginUid);
}
