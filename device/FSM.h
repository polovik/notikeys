#ifndef FSM_H
#define FSM_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QPair>

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

    enum buttonState {
        KEY_UNKNOWN      = 99,
        KEY_DISCONNECTED = 11,
        KEY_CONNECTED    = 22,
        KEY_PRESSED      = 33,
        KEY_RELEASED     = 44
    };
    Q_ENUMS(buttonState)

public slots:
    void start();

signals:
    void deviceAppeared();
    void deviceDisappeared();
    void buttonStateChanged(int uid, buttonState state, int pos);

private slots:
    void displayStateName(QString name);
    void indicateDeviceDetection();
    void notifyButtonsState(QMap<int, QPair<int, int> > states);

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
    QMap<int, QPair<int, int> > m_buttonsStates;
};

#endif // FSM_H
