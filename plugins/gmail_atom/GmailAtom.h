#ifndef GMAILATOM_H
#define GMAILATOM_H

#include <QObject>
#include <QtPlugin>
#include <QQueue>
#include <QPair>
#include <QMutex>
#include <QTimer>
#include <gui/PluginInterface.h>

class QNetworkAccessManager;
class QNetworkReply;
class QAuthenticator;
class QQmlContext;

class GmailAtom : public QObject, PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "notikeys.gui.qt.PluginInterface" FILE "gmail_atom.json")
    Q_INTERFACES(PluginInterface)

public:
    GmailAtom(QObject *parent = 0);
    ~GmailAtom();

    void loadPlugin();
    void exportToQML(QQmlContext *context);

public slots:
    void start();
    void stop();
    void verifyAccount(const QString &user, const QString &password);

private slots:
    void provideAuthenication(QNetworkReply *reply, QAuthenticator *ator);
    void readData(QNetworkReply *reply);
    void fetchFeed();

private:
    int parseXml(QByteArray data);

    QNetworkAccessManager *m_manager;
    QQueue<QPair<QString, QString> > m_queueTestAuth;
    QPair<QString, QString> m_lastTestAuth;
    QMutex m_authMutex;
    QTimer m_pollingTimer;

signals:
    void error(QString description);
    void feedLoaded(int newMessagesCount);
    void allCredentialsTested(bool inProgress);
};

#endif // GMAILATOM_H
