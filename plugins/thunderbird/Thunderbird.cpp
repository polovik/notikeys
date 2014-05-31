#include <QDebug>
#include <QQmlContext>
#include "Thunderbird.h"
#include "device/Settings.h"

Thunderbird::Thunderbird(QObject *parent) : QObject(parent)
{
}

Thunderbird::~Thunderbird()
{
}

void Thunderbird::loadPlugin()
{
    m_addonPresenceTimer.setSingleShot(true);
    m_addonPresenceTimer.setInterval(ADDON_PRESENCE_TIMEOUT_MS);
    connect(&m_addonPresenceTimer, SIGNAL(timeout()), this, SIGNAL(addonIsAbsent()));
}

void Thunderbird::exportToQML(QQmlContext *context)
{
    context->setContextProperty("Thunderbird", this);
}

void Thunderbird::start()
{
    qDebug() << "Start Thunderbird's plugin";
    m_addonPresenceTimer.start();
}

void Thunderbird::stop()
{
    m_addonPresenceTimer.stop();
}

void Thunderbird::check()
{
    qDebug() << "Check presence of addon for Thunderbird";
    m_addonPresenceTimer.start();
}


void Thunderbird::analizeExternalEvents(qint32 eventsCount)
{
    qDebug() << "Recieved from Thunderbird:" << eventsCount << "events";
    m_addonPresenceTimer.start();
    int events = eventsCount;
    emit messagesCount(events);
}
