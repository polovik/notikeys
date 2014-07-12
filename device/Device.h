#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include "../gui/PluginInterface.h"

class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = 0);

public slots:
    void setLedMode(QString pluginUid, LedMode_e mode);

signals:
    void buttonPressed(QString pluginUid);
};

#endif // DEVICE_H
