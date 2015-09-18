#include <QDebug>
#include <QQmlContext>
#include <QProcess>
#include <QSettings>
#include "Skype.h"
#include "device/Settings.h"
#ifdef _WIN32
#include <windows.h>
#include <winuser.h>
#endif

static int m_messages;
#ifdef _WIN32
static HWND m_hwndSkype;
#endif

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

#ifdef _WIN32
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
            m_hwndSkype = hwnd;
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
#endif

void Skype::check()
{
    qDebug() << "Skype::check";
    m_messages = -1;
#ifdef _WIN32
    EnumWindows(&detectSkype, 0);
    if (m_messages == -1) {
        qDebug() << "Skype isn't run";
        emit skypeIsAbsent();
        setLedMode(LED_FREQUENT_BLINK);
    } else if (m_messages == 0) {
        qDebug() << "Skype is present. No new events";
        emit eventsCount(m_messages);
        setLedMode(LED_OFF);
    } else {
        qDebug() << "Skype is present. Events count =" << m_messages;
        emit eventsCount(m_messages);
        setLedMode(LED_RARE_BLINK);
    }
#endif
}

#if 0
static HWND m_hwndTray;

BOOL CALLBACK detectTray(HWND hwnd, LPARAM lParam)
{
    Q_UNUSED(lParam);
    int nMaxCount = 100;
    LPTSTR lpClassName = new TCHAR[nMaxCount];
    int result = GetClassName(hwnd, lpClassName, nMaxCount);
    QString className = QString::fromWCharArray(lpClassName, result);
    delete[] lpClassName;
    if (className == "Shell_TrayWnd") {
        m_hwndTray = hwnd;
        qDebug() << "Detected windows tray area";
        return FALSE;   //  Stop enumeration
    }
    return TRUE; // Continue enumeration
}
#endif

void Skype::handleButtonPressing()
{
    // Branch: HKEY_CURRENT_USER\Software\Skype\Phone Field: SkypePath
    QSettings skypeSettings("Skype", "Phone");
    QString exe = skypeSettings.value("SkypePath").toString();
    exe = "\"" + exe + "\"";

    bool started = QProcess::startDetached(exe);
    qDebug() << "Skype is maximized:" << started;

#if 0
    m_hwndSkype = NULL;
    EnumWindows(&detectSkype, 0);
    if (m_hwndSkype == NULL) {
        qWarning() << "Skype is missing";
        return;
    }

    m_hwndTray = NULL;
    EnumWindows(&detectTray, 0);
    if (m_hwndTray == NULL) {
        qWarning() << "Can't find windows tray area";
        return;
    }
    DWORD hTrayThread = GetWindowThreadProcessId(m_hwndTray, NULL);
    qDebug() << "Tray with handle" << m_hwndTray << "is executed in thread" << hTrayThread;

    HWND hWndCurrentWindow = GetForegroundWindow();
    DWORD hCurrentWindowThread = GetWindowThreadProcessId(hWndCurrentWindow, NULL);
    qDebug() << "Notikeys app has handle" << hWndCurrentWindow << "and is executed in thread" << hCurrentWindowThread;

    ShowWindow(m_hwndSkype, SW_RESTORE);
    AttachThreadInput(hCurrentWindowThread, hTrayThread, TRUE);
    SetActiveWindow(m_hwndSkype);
    BringWindowToTop(m_hwndSkype);
    SetFocus(m_hwndSkype);
    SetForegroundWindow(m_hwndSkype);
    AttachThreadInput(hCurrentWindowThread, hTrayThread, FALSE);
#endif
}
