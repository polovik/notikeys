#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QTimer>
#include "../gui/PluginInterface.h"

class UartPort;
class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = 0);
    ~Device();
    bool init();
    void close();

public slots:
    void setLedMode(QString pluginUid, LedMode_e mode);

signals:
    void buttonPressed(QString pluginUid);
private slots:
    void parsePacket(const QByteArray &rawData);
    void handleResponseTimeout();

private:
    QString m_readBuffer;
    const int TIMEOUT_RESPONSE_MS = 1000;
    UartPort* m_uartPort;
    QTimer m_responseTimer;
};

#endif // DEVICE_H
