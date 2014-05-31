#ifndef THUNDERBIRD_H
#define THUNDERBIRD_H

#include <QObject>
#include <QtPlugin>
#include <QTimer>
#include <gui/PluginInterface.h>

class QQmlContext;

class Thunderbird : public QObject, PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "notikeys.gui.qt.PluginInterface" FILE "thunderbird.json")
    Q_INTERFACES(PluginInterface)

public:
    Thunderbird(QObject *parent = 0);
    ~Thunderbird();

    void loadPlugin();
    void exportToQML(QQmlContext *context);
    void analizeExternalEvents(qint32 eventsCount);

public slots:
    void start();
    void stop();
    void check();

private slots:

private:
    static const int ADDON_PRESENCE_TIMEOUT_MS = 10000; // 2 * 5000("extensions.notikeys.pollTimeout")
    QTimer m_addonPresenceTimer;

signals:
    void addonIsAbsent();
    void messagesCount(int events);
};

#endif // THUNDERBIRD_H
