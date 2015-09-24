#ifndef FSM_H
#define FSM_H

#include <QObject>
#include <QTimer>

class QStateMachine;
class QState;
class Device;
class QSignalTransition;

class StateLogger: public QObject
{
    Q_OBJECT
public:
    StateLogger() {};
    Q_PROPERTY(QString text MEMBER m_text NOTIFY textChanged)
signals:
    void textChanged(const QString &newText);
private:
    QString m_text;
};

class FSM : public QObject
{
    Q_OBJECT
public:
    explicit FSM(QObject *parent = 0);

public slots:

signals:
    void deviceAppeared();
    void deviceDisappeared();

private slots:
    void displayStateName(QString name);
    void indicateDeviceDetection();

private:
    Device *m_device;
    QTimer m_openDeviceTimer;
    QTimer m_devicePresenceTimer;
    QTimer m_quitApplicationTimer;

    QStateMachine *m_machine;
    QState *m_stateSTARTING;
    QState *m_stateFINDING_DEVICE;
    QState *m_stateHAND_SHAKING;
    QState *m_stateREADY;
    QState *m_stateSTOP_DEVICE;
    QState *m_stateEXIT;
    QSignalTransition *m_transitionDeviceNotResponded;

    StateLogger m_enteredStateName;
};

#endif // FSM_H
