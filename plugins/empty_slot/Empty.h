#ifndef EMPTY_H
#define EMPTY_H

#include <QObject>
#include <QtPlugin>
#include <gui/PluginInterface.h>

class QQmlContext;

class EmptySlot : public QObject, PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "notikeys.gui.qt.PluginInterface" FILE "empty_slot.json")
    Q_INTERFACES(PluginInterface)

public:
    EmptySlot(QObject *parent = 0);
    ~EmptySlot();

    void loadPlugin();
    void exportToQML(QQmlContext *context);
    void analizeExternalEvents(qint32 eventsCount) { Q_UNUSED(eventsCount); };
    void handleButtonPressing();

public slots:
    void start();
    void stop();

private slots:

private:

signals:
};

#endif // EMPTY_H
