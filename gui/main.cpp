#include <QDebug>
#include <QTranslator>
#include <QTextCodec>
#include <QLocale>
#include <QTime>
#include <QDir>
#include <QQmlContext>
#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"
#include "PluginsManager.h"
#include "ExternalPluginServer.h"
#include "../device/Settings.h"

static QTextCodec *logCodec = NULL;
static FILE *logStream = NULL;
QString g_logFilePath = "";

/** @brief For convenient parsing log files, messages have to be formatted as:
 *      level: message (`placeInSource`)
 *  where:
 *      level - Debug, Warning, Critical, Fatal
 *      message - log message
 *      placeInSource - point, where message was emited in format: (`filename:line, function_signature`)
 */
void logging(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = logCodec->fromUnicode(msg);

    QString fileName(context.file);
    fileName.remove(0, fileName.lastIndexOf("\\") + 1);
    fileName.remove(0, fileName.lastIndexOf("/") + 1);
    QByteArray file = logCodec->fromUnicode(fileName);

    QTime time = QTime::currentTime();
    QString formatedTime = time.toString("hh:mm:ss.zzz");
    fprintf(logStream, "%s ", qPrintable(formatedTime));

    switch (type) {
    case QtDebugMsg:
        fprintf(logStream, "Debug: %s (`%s:%u, %s`)\n", localMsg.constData(), file.constData(), context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(logStream, "Warning: %s (`%s:%u, %s`)\n", localMsg.constData(), file.constData(), context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(logStream, "Critical: %s (`%s:%u, %s`)\n", localMsg.constData(), file.constData(), context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(logStream, "Fatal: %s (`%s:%u, %s`)\n", localMsg.constData(), file.constData(), context.line, context.function);
        abort();
        break;
    }
    fflush(logStream);
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    //  Configure and redirect log output to stderr or in text file
    QByteArray envVar = qgetenv("QTDIR");   //  this variable is only set when run application in QtCreator
    if (envVar.isEmpty()) {
        g_logFilePath = QDir::tempPath() + QDir::separator() + "log_notikeys.txt";
        logStream = _wfopen(g_logFilePath.toStdWString().c_str(), L"w");
    } else {
        logStream = stderr;
    }
    logCodec = QTextCodec::codecForName("Windows-1251");
    qInstallMessageHandler(logging);
    qDebug() << "Start notikeys";

    QtQuick2ApplicationViewer viewer;

    //  Set the organization name and the application name for using QSettings
    app.setOrganizationName("Horns&Hoofs Inc.");
    app.setApplicationName("Notikeys");

    PluginsManager pluginsManager(viewer.rootContext());
    viewer.rootContext()->setContextProperty("PluginsManager", &pluginsManager);

    ExternalPluginServer pluginServer;
    pluginServer.startServer();

    Settings settings;
    viewer.rootContext()->setContextProperty("Settings", &settings);

    QQmlImageProviderBase *imageProvider = new PluginsImageProvider(&pluginsManager);
    viewer.engine()->addImageProvider(QLatin1String("pluginlogo"), imageProvider);

    viewer.setMainQmlFile(QStringLiteral("qml/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
