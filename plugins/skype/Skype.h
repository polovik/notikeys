#ifndef SKYPE_H
#define SKYPE_H

#include <QObject>
#include <QtPlugin>
#include <QTimer>
#include <gui/PluginInterface.h>

class QQmlContext;

class Skype : public QObject, PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "notikeys.gui.qt.PluginInterface" FILE "skype.json")
    Q_INTERFACES(PluginInterface)

public:
    Skype(QObject *parent = 0);
    ~Skype();

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
    void skypeIsAbsent();
    void eventsCount(int events);
};

#endif // SKYPE_H
