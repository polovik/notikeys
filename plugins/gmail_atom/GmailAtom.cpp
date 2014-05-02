#include "QDebug"
#include "QFile"
#include "QImage"
#include "GmailAtom.h"

QImage GmailAtom::buttonImage() const
{
    QImage image;
    bool loaded = image.load("qrc:/gmail.png");
    if (!loaded)
        qCritical() << "Can't read button image";

    return image;
}

QByteArray GmailAtom::settingsScreenQml() const
{
    QFile file("qrc:/AccountSettings.qml");
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Settings QML screen can't be readed";
        return QByteArray();
    }

    return file.readAll();
}
