#ifndef FSM_H
#define FSM_H

#include <QObject>
#include <QTimer>
#include <QLineEdit>

class QStateMachine;
class QState;
class Device;
class MainWindow;
class QSignalTransition;

class FSM : public QObject
{
    Q_OBJECT
public:
    explicit FSM(MainWindow *mainWindow, QObject *parent = 0);

public slots:

signals:
    void SIG_DEVICE_CHECKING_TIMEOUT();

private slots:
    void displayStateName(QString name);
    void ejectKLineTests(bool eject);
    void deviceStatusTimeout();

private:
    MainWindow *m_guiMainWindow;
    Device *m_device;
    QTimer m_openDeviceTimer;
    QTimer m_devicePresenceTimer;
    QTimer m_quitApplicationTimer;

    QStateMachine *m_machine;
    //  Initialize/Finalize stage
    QState *m_stateSTARTING;
    QState *m_stateFINDING_DEVICE;
    QState *m_stateSTOP_DEVICE;
    QState *m_stateEXIT;
    //  Selecting Pump phase
    QState *m_stateWORKING_PHASE;   // - Meta-State
    QState *m_stateHAND_SHAKING;
    QState *m_stateCONNECTING;
    QState *m_stateCONNECTED;
    QState *m_stateCALIBRATION;
    QState *m_stateSTOP_CALIBRATION;
    QState *m_stateCALIBRATED;

    QState *m_stateWRITTING_SETTINGS;
    QState *m_stateTESTING;
    QState *m_stateSTOP_TESTING;
    QState *m_stateTESTED;

    QState *m_stateWAIT_ACCEPTANCE;
    QState *m_statePROGRAMMING;
    QState *m_statePROGRAMMED;
    QState *m_stateFLASHING;
    QState *m_stateFLASHED;

    QLineEdit m_enteredStateName;
};

#endif // FSM_H
