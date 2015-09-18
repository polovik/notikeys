#include <QDebug>
#include <QStateMachine>
#include <QState>
#include <QSignalTransition>
#include "FSM.h"
#include "Device.h"
#include "MainWindow.h"

FSM::FSM(MainWindow *mainWindow, QObject *parent) :
    QObject(parent), m_guiMainWindow(mainWindow)
{
    m_device = new Device;
//    connect(m_device, SIGNAL(pumpIdReaded(QStringList)), m_guiMainWindow, SLOT(displayPumpId(QStringList)));
//    connect(m_device, SIGNAL(pumpErrorsReaded(QList<int>)), m_guiMainWindow, SLOT(displayPumpErrors(QList<int>)));
//    connect(m_device, SIGNAL(notificationCAN(quint8,quint32,QByteArray)), m_guiMainWindow, SLOT(updateScales(quint8,quint32,QByteArray)));
//    connect(m_device, SIGNAL(adcValues(QVector<double>)), m_guiMainWindow, SLOT(displayADCvalues(QVector<double>)));
//    connect(m_guiMainWindow, SIGNAL(canConfigUpdated(quint32,quint8,quint8,quint16,quint8,QList<quint32>)),
//            m_device, SLOT(setPumpConfig(quint32,quint8,quint8,quint16,quint8,QList<quint32>)));
//    connect(m_guiMainWindow, SIGNAL(skipKLineTests(bool)), this, SLOT(ejectKLineTests(bool)));
//    connect(m_guiMainWindow, SIGNAL(canBindingInfoUpdated(QList<CANstreamInfo_s>)),
//            m_device, SLOT(setBindingInfo(QList<CANstreamInfo_s>)));

    m_machine = new QStateMachine;
    /*  Initialize/Finalize stage   */
    m_stateSTARTING = new QState;
    m_stateFINDING_DEVICE = new QState;
    m_stateSTOP_DEVICE = new QState;
    m_stateEXIT = new QState;
    /*  Working phase    */
    m_stateWORKING_PHASE = new QState; // - Meta-State
    m_stateHAND_SHAKING = new QState(m_stateWORKING_PHASE);
    m_stateCONNECTING = new QState(m_stateWORKING_PHASE);
    m_stateCONNECTED = new QState(m_stateWORKING_PHASE);
    m_stateCALIBRATION = new QState(m_stateWORKING_PHASE);
    m_stateSTOP_CALIBRATION = new QState(m_stateWORKING_PHASE);
    m_stateCALIBRATED = new QState(m_stateWORKING_PHASE);
    m_stateWRITTING_SETTINGS = new QState(m_stateWORKING_PHASE);
    m_stateTESTING = new QState(m_stateWORKING_PHASE);
    m_stateSTOP_TESTING = new QState(m_stateWORKING_PHASE);
    m_stateTESTED = new QState(m_stateWORKING_PHASE);

    m_stateWAIT_ACCEPTANCE = new QState(m_stateWORKING_PHASE);
    m_statePROGRAMMING = new QState(m_stateWORKING_PHASE);
    m_statePROGRAMMED = new QState(m_stateWORKING_PHASE);
    m_stateFLASHING = new QState(m_stateWORKING_PHASE);
    m_stateFLASHED = new QState(m_stateWORKING_PHASE);


    /*  Initialize/Finalize stage   */
    m_stateSTARTING->assignProperty(&m_enteredStateName, "text", "STARTING");
    m_stateFINDING_DEVICE->assignProperty(&m_enteredStateName, "text", "FINDING_DEVICE");
    m_stateSTOP_DEVICE->assignProperty(&m_enteredStateName, "text", "STOP_DEVICE");
    m_stateEXIT->assignProperty(&m_enteredStateName, "text", "EXIT");
    /*  Working phase    */
    m_stateHAND_SHAKING->assignProperty(&m_enteredStateName, "text", "HAND_SHAKING");
    m_stateCONNECTING->assignProperty(&m_enteredStateName, "text", "CONNECTING");
    m_stateCONNECTED->assignProperty(&m_enteredStateName, "text", "CONNECTED");
    m_stateCALIBRATION->assignProperty(&m_enteredStateName, "text", "CALIBRATION");
    m_stateSTOP_CALIBRATION->assignProperty(&m_enteredStateName, "text", "STOP_CALIBRATION");
    m_stateCALIBRATED->assignProperty(&m_enteredStateName, "text", "CALIBRATED");
    m_stateWRITTING_SETTINGS->assignProperty(&m_enteredStateName, "text", "WRITTING_SETTINGS");
    m_stateTESTING->assignProperty(&m_enteredStateName, "text", "TESTING");
    m_stateSTOP_TESTING->assignProperty(&m_enteredStateName, "text", "STOP_TESTING");
    m_stateTESTED->assignProperty(&m_enteredStateName, "text", "TESTED");
    m_stateWAIT_ACCEPTANCE->assignProperty(&m_enteredStateName, "text", "WAIT_ACCEPTANCE");
    m_statePROGRAMMING->assignProperty(&m_enteredStateName, "text", "PROGRAMMING");
    m_statePROGRAMMED->assignProperty(&m_enteredStateName, "text", "PROGRAMMED");
    m_stateFLASHING->assignProperty(&m_enteredStateName, "text", "FLASHING");
    m_stateFLASHED->assignProperty(&m_enteredStateName, "text", "FLASHED");

    connect(&m_enteredStateName, SIGNAL(textChanged(QString)), this, SLOT(displayStateName(QString)));

    /*  Initialize/Finalize stage   */
    //============= STARTING ========================//
    connect(m_stateSTARTING, SIGNAL(entered()), m_guiMainWindow, SLOT(show()));
    m_stateSTARTING->addTransition(m_guiMainWindow, SIGNAL(SIG_START()), m_stateFINDING_DEVICE);

    //============= FINDING_DEVICE ==================//
    m_openDeviceTimer.setInterval(1000);
    connect(m_stateFINDING_DEVICE, SIGNAL(entered()), &m_openDeviceTimer, SLOT(start()));
    connect(m_stateFINDING_DEVICE, SIGNAL(entered()), m_device, SLOT(openDevice()));
    connect(m_stateFINDING_DEVICE, SIGNAL(entered()), m_guiMainWindow, SLOT(displayDeviceDisconnected()));
    m_stateFINDING_DEVICE->addTransition(&m_openDeviceTimer, SIGNAL(timeout()), m_stateFINDING_DEVICE);
    m_stateFINDING_DEVICE->addTransition(m_device, SIGNAL(SIG_DEVICE_OPENED()), m_stateWORKING_PHASE);
    connect(m_stateFINDING_DEVICE, SIGNAL(exited()), &m_openDeviceTimer, SLOT(stop()));

    //============= STOP_DEVICE =====================//
    m_quitApplicationTimer.setInterval(1000);
    connect(m_stateSTOP_DEVICE, SIGNAL(entered()), m_device, SLOT(requestReset()));
    connect(m_stateSTOP_DEVICE, SIGNAL(entered()), &m_quitApplicationTimer, SLOT(start()));
    m_stateSTOP_DEVICE->addTransition(&m_quitApplicationTimer, SIGNAL(timeout()), m_stateEXIT);

    //============= EXIT ============================//
    connect(m_stateEXIT, SIGNAL(entered()), m_device, SLOT(closeDevice()));
    connect(m_stateEXIT, SIGNAL(entered()), m_guiMainWindow, SLOT(close()));

    /*  Selecting Pump phase    */
    //============= WORKING_PHASE ===================//
    connect(m_stateWORKING_PHASE, SIGNAL(entered()), m_guiMainWindow, SLOT(displayDeviceConnected()));
    m_stateWORKING_PHASE->addTransition(m_device, SIGNAL(SIG_DEVICE_CLOSED()), m_stateFINDING_DEVICE);
    m_stateWORKING_PHASE->addTransition(m_guiMainWindow, SIGNAL(SIG_ABOUT_TO_QUIT()), m_stateSTOP_DEVICE);

    //============= HAND_SHAKING ====================//
    connect(m_stateHAND_SHAKING, SIGNAL(entered()), m_device, SLOT(requestHandshake()));
    m_stateHAND_SHAKING->addTransition(m_device, SIGNAL(SIG_HANDSHAKED()), m_stateCONNECTING);

    //============= CONNECTING ======================//
    connect(m_stateCONNECTING, SIGNAL(entered()), m_device, SLOT(requestConnect()));
    m_stateCONNECTING->addTransition(m_device, SIGNAL(SIG_CONNECTED()), m_stateCONNECTED);

    //============= CONNECTED =======================//
    connect(m_stateCONNECTED, SIGNAL(entered()), m_guiMainWindow, SLOT(unlockCalibrateButton()));
    m_stateCONNECTED->addTransition(m_guiMainWindow, SIGNAL(SIG_CALIBRATION_STARTED()), m_stateCALIBRATION);

    //============= CALIBRATION =====================//
    connect(m_stateCALIBRATION, SIGNAL(entered()), m_device, SLOT(requestStartCalibration()));
    connect(m_stateCALIBRATION, SIGNAL(entered()), m_guiMainWindow, SLOT(lockForCalibration()));
    m_stateCALIBRATION->addTransition(m_guiMainWindow, SIGNAL(SIG_CALIBRATION_FINISHED()), m_stateSTOP_CALIBRATION);

    //============= STOP_CALIBRATION ================//
    connect(m_stateSTOP_CALIBRATION, SIGNAL(entered()), m_device, SLOT(requestStopCalibration()));
    m_stateSTOP_CALIBRATION->addTransition(m_device, SIGNAL(SIG_CALIBRATION_STOPPED()), m_stateCALIBRATED);

    //============= CALIBRATED ======================//
    connect(m_stateCALIBRATED, SIGNAL(entered()), m_guiMainWindow, SLOT(unlockTesting()));
    m_stateCALIBRATED->addTransition(m_guiMainWindow, SIGNAL(SIG_CALIBRATION_STARTED()), m_stateCALIBRATION);
    m_stateCALIBRATED->addTransition(m_guiMainWindow, SIGNAL(SIG_TESTING_STARTED()), m_stateWRITTING_SETTINGS);



    //============= WRITTING_SETTINGS ==============//
    connect(m_stateWRITTING_SETTINGS, SIGNAL(entered()), m_device, SLOT(requestProgramming()));
    connect(m_stateWRITTING_SETTINGS, SIGNAL(entered()), m_guiMainWindow, SLOT(lockForTesting()));
    m_stateWRITTING_SETTINGS->addTransition(m_device, SIGNAL(SIG_SETTINGS_WRITTEN()), m_stateTESTING);

    //============= TESTING ========================//
    connect(m_stateTESTING, SIGNAL(entered()), m_device, SLOT(requestStartTesting()));
    m_stateTESTING->addTransition(m_guiMainWindow, SIGNAL(SIG_TESTING_FINISHED()), m_stateSTOP_TESTING);

    //============= STOP_TESTING ===================//
    connect(m_stateSTOP_TESTING, SIGNAL(entered()), m_device, SLOT(requestStopTesting()));
    m_stateSTOP_TESTING->addTransition(m_device, SIGNAL(SIG_TESTING_STOPPED()), m_stateTESTED);

    //============= TESTED =========================//
    connect(m_stateTESTED, SIGNAL(entered()), m_guiMainWindow, SLOT(unlockFlashing()));
    m_stateTESTED->addTransition(m_guiMainWindow, SIGNAL(SIG_CALIBRATION_STARTED()), m_stateCALIBRATION);
    m_stateTESTED->addTransition(m_guiMainWindow, SIGNAL(SIG_TESTING_STARTED()), m_stateWRITTING_SETTINGS);
    m_stateTESTED->addTransition(m_guiMainWindow, SIGNAL(SIG_REQUEST_PROGRAMMING()), m_stateWAIT_ACCEPTANCE);


    //============= WAIT_ACCEPTANCE ================//
    connect(m_stateWAIT_ACCEPTANCE, SIGNAL(entered()), m_guiMainWindow, SLOT(lockForFlashing()));
    connect(m_stateWAIT_ACCEPTANCE, SIGNAL(entered()), m_guiMainWindow, SLOT(displayReadyToFlash()));
    m_stateWAIT_ACCEPTANCE->addTransition(m_guiMainWindow, SIGNAL(SIG_PROGRAMMING_REJECTED()), m_stateTESTED);
    m_stateWAIT_ACCEPTANCE->addTransition(m_guiMainWindow, SIGNAL(SIG_PROGRAMMING_STARTED()), m_statePROGRAMMING);

    //============= PROGRAMMING ====================//
    connect(m_statePROGRAMMING, SIGNAL(entered()), m_device, SLOT(requestProgramming()));
    m_statePROGRAMMING->addTransition(m_device, SIGNAL(SIG_SETTINGS_WRITTEN()), m_statePROGRAMMED);

    //============= PROGRAMMED =====================//
    connect(m_statePROGRAMMED, SIGNAL(entered()), m_guiMainWindow, SLOT(displayReadyToBurn()));
    m_statePROGRAMMED->addTransition(m_guiMainWindow, SIGNAL(SIG_FLASHING_REJECTED()), m_stateTESTED);
    m_statePROGRAMMED->addTransition(m_guiMainWindow, SIGNAL(SIG_FLASHING_STARTED()), m_stateFLASHING);

    //============= FLASHING =======================//
    connect(m_stateFLASHING, SIGNAL(entered()), m_device, SLOT(requestFlashing()));
    m_stateFLASHING->addTransition(m_device, SIGNAL(SIG_FLASHING_FINISHED()), m_stateFLASHED);

    //============= FLASHED ========================//
    m_devicePresenceTimer.setInterval(1000);
    connect(m_stateFLASHED, SIGNAL(entered()), m_guiMainWindow, SLOT(displayFlashingSuccessed()));
    connect(m_stateFLASHED, SIGNAL(entered()), &m_devicePresenceTimer, SLOT(start()));
    connect(&m_devicePresenceTimer, SIGNAL(timeout()), m_device, SLOT(requestReset()));
    connect(m_stateFLASHED, SIGNAL(exited()), m_guiMainWindow, SLOT(hideFlashingSuccessed()));
    connect(m_stateFLASHED, SIGNAL(exited()), &m_devicePresenceTimer, SLOT(stop()));

    m_machine->addState(m_stateSTARTING);
    m_machine->addState(m_stateFINDING_DEVICE);
    m_machine->addState(m_stateWORKING_PHASE);
    m_machine->addState(m_stateSTOP_DEVICE);
    m_machine->addState(m_stateEXIT);

    m_machine->setInitialState(m_stateSTARTING);
    m_stateWORKING_PHASE->setInitialState(m_stateHAND_SHAKING);

    m_machine->start();
}

void FSM::displayStateName(QString name)
{
    qDebug() << "Enter in state:" << name;
}

void FSM::ejectKLineTests(bool eject)
{
    qDebug() << "Eject KLine tests:" << eject;
//    QList<QAbstractTransition *> transitions = m_stateGETTING_DEVICE_STATUS->transitions();
//    foreach(QAbstractTransition * transition, transitions) {
//        if (eject) {
//            if (transition == m_transitionCheckKLine) {
//                qDebug() << "Remove m_transitionCheckKLine";
//                m_stateGETTING_DEVICE_STATUS->removeTransition(m_transitionCheckKLine);
//                break;
//            }
//        } else {
//            if (transition == m_transitionSkipKLine) {
//                qDebug() << "Remove m_transitionSkipKLine";
//                m_stateGETTING_DEVICE_STATUS->removeTransition(m_transitionSkipKLine);
//                break;
//            }
//        }
//    }
//    if (eject) {
//        m_stateGETTING_DEVICE_STATUS->addTransition(m_transitionSkipKLine);
//    } else {
//        m_stateGETTING_DEVICE_STATUS->addTransition(m_transitionCheckKLine);
//    }
}

void FSM::deviceStatusTimeout()
{
    qDebug() << "timeout";
    emit SIG_DEVICE_CHECKING_TIMEOUT();
}
