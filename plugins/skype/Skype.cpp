#include <QDebug>
#include <QQmlContext>
#include <QProcess>
#include <QSettings>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include "Skype.h"
#include "device/Settings.h"
#ifdef Q_OS_WIN
#include <windows.h>
#include <winuser.h>
#endif

static int m_messages;
#ifdef Q_OS_WIN
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

int getMessagesCount(QString windowName)
{
    // no new messages: yyyyyyy - Skype™
    //  a new messages: [1]yyyyyyy - Skype™
    int numStart = windowName.indexOf("[");
    int numEnd = windowName.indexOf("]");
    int length = numEnd - numStart - 1;
    if ((numStart >= 0) && (numEnd > 0) && (length > 0)) {
        QString events = windowName.mid(numStart + 1, length);
        bool ok = false;
        int count = events.toInt(&ok);
        if (!ok) {
            qCritical() << "Incorrect format of Skype window caption:" << windowName;
            return -1;
        } else {
            return count;
        }
    }
    return 0;
}

#ifdef Q_OS_WIN
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
#elif defined(Q_OS_LINUX)
QString detectSkypeWindowName()
{
    QString windowName = "";
//    # xwininfo -root -tree| grep "[Ss]kype"
//         0x140022c "[1]XXXXXXXXX - Skype™": ("skype" "Skype")  476x856+0+0  +0+0
//         0x1400002 "skype": ("skype" "Skype")  960x432+0+0  +0+0
//               0x1400017 "skype": ("skype" "Skype")  16x16+1404+2  +1404+880
//            0x1400036 "yyyyyyy - Skype™": ("skype" "Skype")  1436x856+2+0  +2+20
//            0x2e0000a "Skype.cpp [master] - notikeys - Qt Creator": ("qtcreator" "QtCreator")  1436x856+2+0  +2+20

    QProcess process;
    process.start("xwininfo", QStringList() << "-root" << "-tree");
    if (!process.waitForFinished(1000)) {
        qCritical() << "Couldn't enumerate X windows in time";
        return windowName;
    }
    QByteArray result = process.readAll();
    QList<QByteArray> lines = result.split('\n');
    QList<QString> windows;
    foreach (QByteArray line, lines) {
        if (line.contains("(\"skype\" \"Skype\")")) {
            windows.append(line.trimmed());
        }
    }
    foreach (QString window, windows) {
        int pos = window.indexOf(" ");
//        QString id = window.left(pos);
        int endPos = window.indexOf("\"", pos + 2);
        int len = endPos - (pos + 2);
        QString name = window.mid(pos + 2, len);
        if (name.isEmpty())
            continue;
        if (!name.contains("- Skype™"))
            continue;
        if (getMessagesCount(name) > 0) {
            return name;
        }
        if (!windowName.isEmpty()) {
//            qWarning() << "Several X windows are conditates to main Skype window:" << windowName << "and" << name;
        }
        windowName = name;
    }
    return windowName;
}
#endif

void Skype::check()
{
    qDebug() << "Skype::check";
    QString windowName = "";
#ifdef Q_OS_WIN
    m_messages = -1;
    EnumWindows(&detectSkype, 0);
#elif defined(Q_OS_LINUX)
    windowName = detectSkypeWindowName();
#endif
    qDebug() << "windowName:" << windowName;
    if (windowName.isEmpty()) {
        qDebug() << "Skype isn't run";
        emit skypeIsAbsent();
        setLedMode(LED_FREQUENT_BLINK);
        return;
    }
    m_messages = getMessagesCount(windowName);
    if (m_messages == -1) {
        qWarning() << "Couldn't detect number of a new messages in Skype";
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
