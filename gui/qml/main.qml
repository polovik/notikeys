import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import FSMPackage 1.0

Rectangle {
    id: mainScreen
    width: 500
    height: 200

    state: "INIT"
    states: [
        State {
            name: "INIT"
            PropertyChanges { target: screenInitialization; x: 0; z: 10}
            PropertyChanges { target: screenDisconnected; x: 0; z: 1; deviceConnected: false}
            PropertyChanges { target: screenConnected; x: 0; z: 1}
        },
        State {
            name: "DISCONNECTED"
            PropertyChanges { target: screenInitialization; x: 0; z: 1}
            PropertyChanges { target: screenDisconnected; x: 0; z: 10; deviceConnected: false }
            PropertyChanges { target: screenConnected; x: 0; z: 1}
        },
        State {
            name: "ACTIVE"
            PropertyChanges { target: screenInitialization; x: 0; z: 1}
            PropertyChanges { target: screenDisconnected; x: width; z: 1; deviceConnected: true }
            PropertyChanges { target: screenConnected; z: 10}
        }
    ]
    transitions: [
        Transition {
            from: "DISCONNECTED"
            to: "ACTIVE"
            SequentialAnimation {
                PauseAnimation  { duration: 500 }
                NumberAnimation { target: screenDisconnected; properties: "x"; easing.type: Easing.InBack; duration: 500 }
                NumberAnimation { targets: [screenConnected, screenDisconnected]; property: "z" }
            }
        },
        Transition {
            from: "ACTIVE"
            to: "DISCONNECTED"
            SequentialAnimation {
                NumberAnimation { targets: [screenConnected, screenDisconnected]; property: "z" }
                NumberAnimation { target: screenDisconnected; properties: "x"; easing.type: Easing.OutBack; duration: 500 }
            }
        }
    ]

    Rectangle {
        id: screenInitialization
        x: 0
        y: 0
        width: parent.width
        height: parent.height
        color: "black"
        AnimatedImage {
            id: imageInit
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height * 0.5
            fillMode: Image.PreserveAspectFit
            source: "images/initialization.gif"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                mainScreen.state = "DISCONNECTED"
            }
        }
    }

    Rectangle {
        id: screenDisconnected
        x: 0
        y: 0
        width: parent.width
        height: parent.height
        property bool deviceConnected: false
        color: {
            if (deviceConnected)
                return "#41c445"
            else
                return "#ff9807"
        }
        Image {
            id: imageDisconnected
            anchors.left: parent.left
            anchors.leftMargin: parent.width * 0.05
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width * 0.3
            fillMode: Image.PreserveAspectFit
            source: {
                if (screenDisconnected.deviceConnected)
                    return "images/connect.png"
                else
                    return "images/disconnect.png"
            }
        }
        Text {
            id: labelStatus
            anchors.left: imageDisconnected.right
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: ""
            function updateText() {
                if (screenDisconnected.deviceConnected)
                    text = qsTr("Notikeys is connected!")
                else
                    text = qsTr("Notikeys is disconnected.\nPlug it in USB port")
            }
            Connections {
                target: mainScreen
                onLanguageChanged: labelStatus.updateText()
            }
            Connections {
                target: screenDisconnected
                onDeviceConnectedChanged: labelStatus.updateText()
            }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                mainScreen.state = "ACTIVE"
            }
        }
    }

    PluginSettingsScreen {
        id: screenPluginConfigure
        x: 0
        y: 0
        width: parent.width
        height: parent.height
        onClosed: {
            PluginsManager.stopPlugins()
            PluginsManager.startActivePlugins()
        }
    }

    property bool viewInactiveButtons: false
    property string language: ""

    Rectangle {
        id: screenConnected
        x: 0
        y: 0
        width: parent.width
        height: parent.height
        color: "gold"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                mainScreen.state = "DISCONNECTED"
            }
        }
        CheckBox {
            id: checkBoxViewInactiveButtons
            anchors.left: parent.left
            anchors.leftMargin: parent.width * 0.01
            anchors.bottom: parent.bottom
            anchors.bottomMargin: parent.width * 0.01
            checked: false
            style: CheckBoxStyle {
                indicator: Rectangle {
                    implicitWidth: 16
                    implicitHeight: 16
                    radius: 3
                    border.color: control.activeFocus ? "darkblue" : "gray"
                    border.width: 1
                    Rectangle {
                        visible: control.checked
                        color: "#555"
                        border.color: "#333"
                        radius: 1
                        anchors.margins: 4
                        anchors.fill: parent
                    }
                }
                label: Text {
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: ""
                    Connections {
                        target: mainScreen
                        onLanguageChanged: {
                            text = qsTr("Inactive buttons")
                        }
                    }
                }
            }
            onCheckedChanged: {
                console.log("View inactive buttons: " + checked)
                viewInactiveButtons = checked
            }
        }
        Image {
            id: imageSettings
            anchors.right: parent.right
            anchors.rightMargin: parent.width * 0.01
            anchors.bottom: parent.bottom
            anchors.bottomMargin: parent.width * 0.01
            height: parent.height * 0.3
            fillMode: Image.PreserveAspectFit
            source: "images/settings.png"
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log("Display settings")
                }
            }
        }
        Image {
            id: imageLanguage
            anchors.right: imageSettings.left
            anchors.rightMargin: parent.width * 0.01
            anchors.bottom: imageSettings.bottom
            height: parent.height * 0.3
            fillMode: Image.PreserveAspectFit
            source: ""
            Connections {
                target: mainScreen
                onLanguageChanged: {
                    console.log("Selected language: " + mainScreen.language)
                    if (language === "us_US")
                        imageLanguage.source = "images/lang_us_US.png"
                    else
                        imageLanguage.source = "images/lang_ru_RU.png"
                    PluginsManager.loadLanguage(mainScreen.language)
                    Settings.set("language", language)
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (language === "us_US")
                        language = "ru_RU"
                    else
                        language = "us_US"
                }
            }
        }

        property int buttonOffset: width * 0.1
        property int buttonWidth: (width - buttonOffset * 4) / 3

        Item {
            id: pluginsButtonsArea
            anchors.left: parent.left
            anchors.leftMargin: screenConnected.buttonOffset
            anchors.top: parent.top
            anchors.topMargin: (imageSettings.y - screenConnected.buttonWidth) / 2
            width: screenConnected.buttonWidth * 3 + screenConnected.buttonOffset * 2
            height: screenConnected.buttonWidth
            Row {
                //  Display "places" for possible active buttons
                spacing: screenConnected.buttonOffset
                Repeater {
                    model: 3
                    Rectangle {
                        width: screenConnected.buttonWidth
                        height: screenConnected.buttonWidth
                        color: "transparent"
                        border.color: "white"
                        border.width: 1
                        radius: 10
                    }
                }
            }

            Row {
                //  Display buttons for configure plugins and display status of plugins - active or not
                spacing: screenConnected.buttonOffset
                Repeater {
                    model: pluginsModel
                    //  this model has fields:
                    //  - uid
                    //  - name
                    //  - settingsScreenPath
                    //  - active
                    Rectangle {
                        enabled: {
                            if (viewInactiveButtons || active)
                                return true
                            else
                                return false
                        }
                        visible: {
                            if (viewInactiveButtons || active)
                                return true
                            else
                                return false
                        }
                        width: {
                            if (viewInactiveButtons || active)
                                return screenConnected.buttonWidth
                            else
                                return 0
                        }
                        height: screenConnected.buttonWidth
                        color: "transparent"
                        border.color: {
                            if (active)
                                return "black"
                            else
                                return "white"
                        }
                        border.width: {
                            if (active)
                                return 2
                            else
                                return 1
                        }
                        radius: 10
                        Image {
                            id: pluginLogo
                            anchors.fill: parent
                            anchors.centerIn: parent
                            fillMode: Image.PreserveAspectFit
                            source: "image://pluginLogo/" + uid
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                console.log("Button #0 is pressed")
                                PluginsManager.stopPlugins()
                                screenPluginConfigure.pluginId = uid;
                                screenPluginConfigure.pluginName = name;
                                screenPluginConfigure.makeVisible(settingsScreenPath)
                            }
                        }
                    }
                }
            }
        }
    }

    function handleDeviceConnected() {
        console.log("Device is connected")
        mainScreen.state = "ACTIVE"
    }

    function handleDeviceDisconnected() {
        console.log("Device is disconnected")
        mainScreen.state = "DISCONNECTED"
    }

    function handleButtonState(uid, state, pos) {
        var st = ""
        switch (state) {
        case FSM.KEY_DISCONNECTED:
            st = "disconnected"
            break;
        case FSM.KEY_CONNECTED:
            st = "connected"
            break;
        case FSM.KEY_PRESSED:
            st = "pressed"
            break;
        case FSM.KEY_RELEASED:
            st = "released"
            break;
        default:
            break;
        }
        console.log("Button " + uid + " changed state to '" + st + "' at pos " + pos)
        if (state === FSM.KEY_PRESSED) {
            PluginsManager.processButtonPressing(uid)
        }
        if (state === FSM.KEY_CONNECTED) {
            PluginsManager.activatePlugin(uid, pos)
        }
        if (state === FSM.KEY_DISCONNECTED) {
            PluginsManager.deactivatePlugin(uid, pos)
        }
    }

    Component.onCompleted: {
        screenPluginConfigure.makeInvisible()
        PluginsManager.loadPlugins();
        var lang = Settings.get("language")
        if (lang.length === 0)
            mainScreen.language = "us_US"
        else
            mainScreen.language = lang
        DeviceFSM.deviceAppeared.connect(handleDeviceConnected)
        DeviceFSM.deviceDisappeared.connect(handleDeviceDisconnected)
        DeviceFSM.buttonStateChanged.connect(handleButtonState)
        DeviceFSM.start()
    }
}
