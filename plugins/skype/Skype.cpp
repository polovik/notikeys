#include <QDebug>
#include <QQmlContext>
#include "skype.h"
#include "device/Settings.h"
#include <windows.h>
#include <winuser.h>

static int m_messages;

Skype::Skype(QObject *parent) : QObject(parent)
{
}

Skype::~Skype()
{
}

void Skype::loadPlugin()
{
    m_messages = -1;
    connect(&m_pollingTimer, SIGNAL(timeout()), this, SLOT(check()));
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

BOOL CALLBACK detectSkype(HWND hwnd, LPARAM lParam)
{
    Q_UNUSED(lParam);
    int nMaxCount = 100;
    LPTSTR lpClassName = new TCHAR[nMaxCount];
    int result = GetClassName(hwnd, lpClassName, nMaxCount);
    QString className = QString::fromWCharArray(lpClassName, result);
    delete[] lpClassName;
    if (className == "tSkMainForm") {
        LPTSTR lpWindowText = new TCHAR[nMaxCount];
        result = GetWindowText(hwnd, lpWindowText, nMaxCount);
        QString windowText = QString::fromWCharArray(lpWindowText, result);
        delete[] lpWindowText;
        qDebug() << windowText;
        if (windowText.contains("Skype")) {
            m_messages = 0;
            int numStart = windowText.indexOf("[");
            int numEnd = windowText.indexOf("]");
            int length = numEnd - numStart - 1;
            if ((numStart > 0) && (numEnd > 0) && (length > 0)) {
                QString events = windowText.mid(numStart + 1, length);
                bool ok = false;
                int count = events.toInt(&ok);
                if (!ok) {
                    qCritical() << "Incorrect format of Skype window caption";
                } else {
                    m_messages = count;
                }
            }
            return FALSE;   //  Stop enumeration
        }
    }
    return TRUE; // Continue enumeration
}

void Skype::check()
{
    qDebug() << "Skype::check";
    m_messages = -1;
    EnumWindows(&detectSkype, 0);
    if (m_messages == -1) {
        qDebug() << "Skype isn't run";
        emit skypeIsAbsent();
    } else if (m_messages == 0) {
        qDebug() << "Skype is present. No new events";
        emit eventsCount(m_messages);
    } else {
        qDebug() << "Skype is present. Events count =" << m_messages;
        emit eventsCount(m_messages);
    }
}
