#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>

typedef enum {
    LED_OFF = 0,
    LED_ON  = 1,
    LED_SINGLE_BLINK    = 2,
    LED_RARE_BLINK      = 3,
    LED_FREQUENT_BLINK  = 4,
    LED_BRIGHTNESS_GLIDING  = 5
} LedMode_e;

class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = 0);

public slots:
    void setLedMode(qint32 pluginUid, LedMode_e mode);

signals:
    void buttonPressed(qint32 pluginUid);
};

#endif // DEVICE_H
