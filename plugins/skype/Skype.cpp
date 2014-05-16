#include <QDebug>
#include <QQmlContext>
#include "skype.h"
#include "device/Settings.h"

Skype::Skype(QObject *parent) : QObject(parent)
{
}

Skype::~Skype()
{
}

void Skype::loadPlugin()
{
    connect(&m_pollingTimer, SIGNAL(timeout()), this, SLOT(fetchFeed()));
}

void Skype::exportToQML(QQmlContext *context)
{
    context->setContextProperty("Skype", this);
}

void Skype::start()
{
    Settings settings;
    QString interval = settings.get("Skype/pollingInterval");
    bool ok = false;
    int seconds = interval.toInt(&ok);
    if (ok == false) {
        qWarning() << "Polling interval is missed or incorrect:" << interval;
        return;
    }
    qDebug() << "Start polling Skype every" << seconds << "sec.";
    m_pollingTimer.start(seconds * 1000);
}

void Skype::stop()
{
    m_pollingTimer.stop();
}
