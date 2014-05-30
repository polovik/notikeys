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

public slots:
    void start();
    void stop();
    void check();

private slots:

private:
    QTimer m_pollingTimer;

signals:
    void addonIsAbsent();
    void eventsCount(int events);
};

#endif // THUNDERBIRD_H
