#include <QDebug>
#include <QSettings>
#include "Settings.h"

Settings::Settings(QObject *parent) :
    QObject(parent)
{
}

/** @brief Store value in registry field - key.
 */
void Settings::set(QString key, QString value)
{
    QSettings settings;
    settings.setValue(key, value);
}

/** @brief Extract value for specified key.
 *  @return value on success, otherwise return "".
 */
QString Settings::get(QString key)
{
    QSettings settings;

    if (settings.contains(key) != true) {
        qCritical() << "Registry path" << key << "doesn't exist";
        return "";
    }

    QString str = settings.value(key).toString();
    return str;
}
