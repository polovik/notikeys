#include <QDebug>
#include <QStateMachine>
#include <QState>
#include <QSignalTransition>
#include "FSM.h"
#include "Device.h"

FSM::FSM(QObject *parent) :
    QObject(parent)
{
    m_device = new Device;
    connect(m_device, SIGNAL(buttonsState(QMap<int,QPair<int,int> >)), this, SLOT(notifyButtonsState(QMap<int,QPair<int,int> >)));

    m_machine = new QStateMachine;
    m_stateSTARTING = new QState;
    m_stateFINDING_DEVICE = new QState;
    m_stateHAND_SHAKING = new QState;
    m_stateREADY = new QState;
    m_stateSTOP_DEVICE = new QState;
    m_stateEXIT = new QState;

    m_stateSTARTING->assignProperty(&m_enteredStateName, "text", "STARTING");
    m_stateFINDING_DEVICE->assignProperty(&m_enteredStateName, "text", "FINDING_DEVICE");
    m_stateHAND_SHAKING->assignProperty(&m_enteredStateName, "text", "HAND_SHAKING");
    m_stateREADY->assignProperty(&m_enteredStateName, "text", "READY");
    m_stateSTOP_DEVICE->assignProperty(&m_enteredStateName, "text", "STOP_DEVICE");
    m_stateEXIT->assignProperty(&m_enteredStateName, "text", "EXIT");

    connect(&m_enteredStateName, SIGNAL(textChanged(QString)), this, SLOT(displayStateName(QString)));

    // TODO move this connection to QML??
    connect(this, SIGNAL(deviceAppeared()), this, SLOT(indicateDeviceDetection()));

    /*  Initialize/Finalize stage   */
    //============= STARTING ========================//
//    connect(m_stateSTARTING, SIGNAL(entered()), m_guiMainWindow, SLOT(show()));
//    m_stateSTARTING->addTransition(m_guiMainWindow, SIGNAL(SIG_START()), m_stateFINDING_DEVICE);

    //============= FINDING_DEVICE ==================//
    m_openDeviceTimer.setInterval(1000);
    connect(m_stateFINDING_DEVICE, SIGNAL(entered()), &m_openDeviceTimer, SLOT(start()));
    connect(m_stateFINDING_DEVICE, SIGNAL(entered()), m_device, SLOT(openDevice()));
    m_stateFINDING_DEVICE->addTransition(&m_openDeviceTimer, SIGNAL(timeout()), m_stateFINDING_DEVICE);
    m_stateFINDING_DEVICE->addTransition(m_device, SIGNAL(SIG_DEVICE_OPENED()), m_stateHAND_SHAKING);
    connect(m_stateFINDING_DEVICE, SIGNAL(exited()), &m_openDeviceTimer, SLOT(stop()));

    m_transitionDeviceNotResponded = new QSignalTransition();
    m_transitionDeviceNotResponded->setSenderObject(m_device);
    m_transitionDeviceNotResponded->setSignal(SIGNAL(SIG_DEVICE_NOT_ANSWER()));
    m_transitionDeviceNotResponded->setTargetState(m_stateFINDING_DEVICE);
    connect(m_transitionDeviceNotResponded, SIGNAL(triggered()), m_device, SLOT(closeDevice()));

    //============= HAND_SHAKING ====================//
    connect(m_stateHAND_SHAKING, SIGNAL(entered()), m_device, SLOT(requestHandshake()));
    m_stateHAND_SHAKING->addTransition(m_device, SIGNAL(SIG_HANDSHAKED()), m_stateREADY);
    m_stateHAND_SHAKING->addTransition(m_transitionDeviceNotResponded);

    //============= READY ===========================//
    connect(m_stateREADY, SIGNAL(entered()), SIGNAL(deviceAppeared()));
    m_stateREADY->addTransition(m_device, SIGNAL(SIG_DEVICE_CLOSED()), m_stateFINDING_DEVICE);
    m_stateREADY->addTransition(m_transitionDeviceNotResponded);
    connect(m_stateREADY, SIGNAL(exited()), SIGNAL(deviceDisappeared()));

    //============= STOP_DEVICE =====================//
    m_quitApplicationTimer.setInterval(1000);
    connect(m_stateSTOP_DEVICE, SIGNAL(entered()), m_device, SLOT(requestReset()));
    connect(m_stateSTOP_DEVICE, SIGNAL(entered()), &m_quitApplicationTimer, SLOT(start()));
    m_stateSTOP_DEVICE->addTransition(&m_quitApplicationTimer, SIGNAL(timeout()), m_stateEXIT);

    //============= EXIT ============================//
    connect(m_stateEXIT, SIGNAL(entered()), m_device, SLOT(closeDevice()));
//    connect(m_stateEXIT, SIGNAL(entered()), m_guiMainWindow, SLOT(close()));

    m_machine->addState(m_stateSTARTING);
    m_machine->addState(m_stateFINDING_DEVICE);
    m_machine->addState(m_stateHAND_SHAKING);
    m_machine->addState(m_stateREADY);
    m_machine->addState(m_stateSTOP_DEVICE);
    m_machine->addState(m_stateEXIT);

    m_machine->setInitialState(m_stateFINDING_DEVICE);
}

void FSM::start()
{
    qDebug() << "Start FSM";
    m_machine->start();
}

void FSM::displayStateName(QString name)
{
    qDebug() << "Enter in state:" << name;
}

void FSM::indicateDeviceDetection()
{
    //  Make common enum for protocol and Plugins
    m_device->requestLedCotrol(0, 0x0101);
}

void FSM::notifyButtonsState(QMap<int, QPair<int, int> > states)
{
    QMap<int, QPair<int, int> >::const_iterator i = states.constBegin();
    while (i != states.constEnd()) {
        int pos = i.key();
        QPair<int, int> s = i.value();
        int uid = s.first;
        int state = s.second;
        int prevUid = -1;
        int prevState = KEY_UNKNOWN;
        if (m_buttonsStates.contains(pos)) {
            prevUid = m_buttonsStates[pos].first;
            prevState = m_buttonsStates[pos].second;
        }
//        qDebug() << "Button state:" << pos << uid << state << prevUid << prevState;
        if ((uid == prevUid) && (state == prevState)) {
            ++i;
            continue;
        }
        if (state != prevState) {
            // Connect button
            if (state == BUTTON_MOUNTED) {
                emit buttonStateChanged(uid, KEY_CONNECTED, pos);
            }
            // Disconnect button
            if (state == BUTTON_UNMOUNTED) {
                emit buttonStateChanged(prevUid, KEY_DISCONNECTED, pos);
            }
            // Button has just pressed
            if (state == BUTTON_PRESSED) {
                emit buttonStateChanged(uid, KEY_PRESSED, pos);
            }
            // Button has just released
            if (state == BUTTON_RELEASED) {
                emit buttonStateChanged(uid, KEY_RELEASED, pos);
            }
        }
        ++i;
    }
    m_buttonsStates.clear();
    m_buttonsStates = states;
}

