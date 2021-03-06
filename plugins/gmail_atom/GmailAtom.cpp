#include "QDebug"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QSslSocket>
#include <QSslConfiguration>
#include <QQmlContext>
#include <QXmlStreamReader>
#include <QDesktopServices>
#include <QUrl>
#include "GmailAtom.h"
#include "device/Settings.h"

GmailAtom::GmailAtom(QObject *parent) : QObject(parent)
{
    m_manager = NULL;

}

GmailAtom::~GmailAtom()
{
    if (m_manager != NULL) {
        delete m_manager;
    }
}

void GmailAtom::loadPlugin()
{
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(readData(QNetworkReply *)));
    connect(m_manager, SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)),
            SLOT(provideAuthenication(QNetworkReply *, QAuthenticator *)));
    qDebug() << "SSL support:" << QSslSocket::supportsSsl();
    connect(&m_pollingTimer, SIGNAL(timeout()), this, SLOT(fetchFeed()));
}

void GmailAtom::exportToQML(QQmlContext *context)
{
    context->setContextProperty("GmailAtom", this);
}

void GmailAtom::handleButtonPressing()
{
    qDebug() << "Open mailbox";
    // TODO specify custom account
//    Settings settings;
//    QString user = settings.get("GmailAtom/account");
    QUrl url(QString("https://mail.google.com"), QUrl::TolerantMode);
    bool started = QDesktopServices::openUrl(url);
    if (!started) {
        qCritical() << "Can't start browser and display mailbox";
    }
}

void GmailAtom::start()
{
    Settings settings;
    QString interval = settings.get("GmailAtom/pollingInterval");
    bool ok = false;
    int seconds = interval.toInt(&ok);
    if (ok == false) {
        qWarning() << "Polling interval is missed or incorrect:" << interval;
        return;
    }
    qDebug() << "Start polling Gmail every" << seconds << "sec.";
    m_pollingTimer.start(seconds * 1000);
}

void GmailAtom::stop()
{
    QMutexLocker locker(&m_authMutex);
    m_pollingTimer.stop();
    m_queueTestAuth.clear();
    m_lastTestAuth.first.clear();
    m_lastTestAuth.second.clear();
}

void GmailAtom::fetchFeed()
{
    // Full feed URL: "https://mail.google.com/mail/feed/atom");
    QUrl url;
    url.setScheme("https");
    url.setHost("mail.google.com");
    url.setPath("/mail/feed/atom");

    QNetworkRequest request(url);
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    qDebug() << "Default SSL configuration isNull:" << config.isNull();
    config.setProtocol(QSsl::SecureProtocols);
    request.setSslConfiguration(config);

    m_manager->get(request);
}

void GmailAtom::verifyAccount(const QString &user, const QString &password)
{
    QMutexLocker locker(&m_authMutex);
    qDebug() << "Try to verify account";
    m_queueTestAuth.enqueue(qMakePair(user, password));
    if (m_queueTestAuth.count() == 1)
        fetchFeed();
}

void GmailAtom::provideAuthenication(QNetworkReply *reply, QAuthenticator *ator)
{
    Q_UNUSED(reply);
    QMutexLocker locker(&m_authMutex);
    QString user;
    QString pass;
    if (m_queueTestAuth.count() >= 1) {
        user = m_queueTestAuth.head().first;
        pass = m_queueTestAuth.head().second;
        if ((m_lastTestAuth.first == user) && (m_lastTestAuth.second == pass)) {
            qWarning() << "Server rejects the credentials. Skip its";
            return;
        }
        m_lastTestAuth.first = user;
        m_lastTestAuth.second = pass;
    } else {
        Settings settings;
        user = settings.get("GmailAtom/account");
        pass = settings.get("GmailAtom/password");
    }
    qDebug() << "Need Authenticator" << m_queueTestAuth.count();
    ator->setUser(user);
    ator->setPassword(pass);
}

void GmailAtom::readData(QNetworkReply *reply)
{
    QMutexLocker locker(&m_authMutex);
    if (!m_queueTestAuth.isEmpty())
        m_queueTestAuth.dequeue();
    m_lastTestAuth.first.clear();
    m_lastTestAuth.second.clear();
    QByteArray data = reply->readAll();

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Reply error" << reply->error() << "Description:" << reply->errorString();
        QString description;
        if (reply->error() == QNetworkReply::AuthenticationRequiredError)
            description = tr("The email or password you entered is incorrect");
        else
            description = reply->errorString();
        emit error(description);
        setLedMode(LED_FREQUENT_BLINK);
    } else {
        int newMessagesCount = parseXml(data);
        qDebug() << "New messages:" << newMessagesCount;
        emit feedLoaded(newMessagesCount);
        if (newMessagesCount > 0)
            setLedMode(LED_RARE_BLINK);
        else
            setLedMode(LED_OFF);
    }
    m_manager->clearAccessCache();
    reply->deleteLater();

    if (m_queueTestAuth.count() >= 1)
        QTimer::singleShot(100, this, SLOT(fetchFeed()));
    else
        emit allCredentialsTested(false);
}

int GmailAtom::parseXml(QByteArray data)
{
    //  Load data in reader
    QXmlStreamReader xml;
    xml.addData(data);

    //  Read xml elements consequently
    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType type;
        type = xml.readNext();

//        qDebug() << "Type is:" << xml.tokenString();
//        qDebug() << "Data:" << xml.qualifiedName() << xml.prefix() << xml.name() << xml.namespaceUri();

        if (type != QXmlStreamReader::StartElement)
            continue;

        if (xml.name() == "fullcount") {
            type = xml.readNext();
            if (type != QXmlStreamReader::Characters) {
                qCritical() << "Incorrect XML structure. Expect count of messages.";
                return 0;
            }
            QString fullcount = xml.text().toString();
            bool ok = false;
            int msgCount = fullcount.toInt(&ok);
            if (!ok) {
                qCritical() << "Incorrect XML structure. Count of messages must be number.";
                return 0;
            }
            return msgCount;
        }
    }

    if (xml.hasError()) {
        qCritical() << "Error during parsing XML:" << xml.errorString() << xml.lineNumber() << xml.columnNumber() << xml.characterOffset();
    }
    qCritical() << "Can't extract count of new messages. XML data:" << data;

    return 0;
}
