#include "QDebug"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QSslSocket>
#include <QSslConfiguration>
#include <QQmlContext>
#include <QTimer>
#include "GmailAtom.h"
//#include "gui/Settings.h"

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
}

void GmailAtom::exportToQML(QQmlContext *context)
{
    context->setContextProperty("GmailAtom", this);
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
        Q_ASSERT(false);
//        Settings settings;
//        user = settings.get("GmailAtom/account");
//        pass = settings.get("GmailAtom/password");
    }
    qDebug() << "Need Authenticator" << m_queueTestAuth.count();
    ator->setUser(user);
    ator->setPassword(pass);
}

void GmailAtom::readData(QNetworkReply *reply)
{
    QMutexLocker locker(&m_authMutex);
    m_queueTestAuth.dequeue();
    m_lastTestAuth.first.clear();
    m_lastTestAuth.second.clear();
    QByteArray data = reply->readAll();

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Reply error" << reply->error() << "Description:" << reply->errorString();
        QString description = reply->errorString();
        emit error(description);
    } else {
        qDebug() << "Reply:" << data;
        int newMessagesCount = 0;
        emit feedLoaded(newMessagesCount);
    }
    m_manager->clearAccessCache();
    reply->deleteLater();

    if (m_queueTestAuth.count() >= 1)
        QTimer::singleShot(100, this, SLOT(fetchFeed()));
}
