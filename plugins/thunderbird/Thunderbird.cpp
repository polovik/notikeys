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
    connect(&m_pollingTimer, SIGNAL(timeout()), this, SLOT(check()));
}

void Thunderbird::exportToQML(QQmlContext *context)
{
    context->setContextProperty("Thunderbird", this);
}

void Thunderbird::start()
{
//    Settings settings;
//    QString interval = settings.get("Thunderbird/pollingInterval");
//    bool ok = false;
//    int seconds = interval.toInt(&ok);
//    if (ok == false) {
//        qWarning() << "Polling interval is missed or incorrect:" << interval;
//        return;
//    }
//    qDebug() << "Start polling Thunderbird every" << seconds << "sec.";
//    m_pollingTimer.start(seconds * 1000);
}

void Thunderbird::stop()
{
//    m_pollingTimer.stop();
}

void Thunderbird::check()
{
    qDebug() << "Thunderbird::check";
//    int m_messages = -1;
//    if (m_messages == -1) {
//        qDebug() << "Thunderbird isn't run";
//        emit addonIsAbsent();
//    } else if (m_messages == 0) {
//        qDebug() << "Thunderbird is present. No new events";
//        emit eventsCount(m_messages);
//    } else {
//        qDebug() << "Thunderbird is present. Events count =" << m_messages;
//        emit eventsCount(m_messages);
//    }
}


void Thunderbird::analizeExternalEvents(qint32 eventsCount)
{
    qDebug() << "Recieved from Thunderbird:" << eventsCount << "events";
}
