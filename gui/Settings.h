#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = 0);

public slots:
    void set(QString key, QString value);
    QString get(QString key);
};

#endif // SETTINGS_H
