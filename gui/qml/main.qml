import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

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
            anchors.left: imageDisconnected.right
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: {
                if (screenDisconnected.deviceConnected)
                    return qsTr("Notikeys is connected!")
                else
                    return qsTr("Notikeys is disconnected.\nPlug it in USB port")
            }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                mainScreen.state = "ACTIVE"
            }
        }
    }

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
                    text: qsTr("Inactive buttons")
                }
            }
            onCheckedChanged: {
                console.log("View inactive buttons: " + checked)
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

        Item {
            id: settingsHeader
            anchors.fill: parent
            anchors.bottomMargin: parent.height * 0.8
            enabled: false
            focus: false
            visible: false
            property string pluginName: ""
            property var mainAreaObject: null

            //  Move popupItem to top of Z stack. Grab focus and enable grabing mouse events
            function makeVisible() {
                z = parent.z + 1
                enabled = true
                focus = true
                visible = true
            }

            //  Hide popupItem and move it to back of all components. Release focus and grabing mouse events
            function makeInvisible() {
                z = 0
                enabled = false
                focus = false
                visible = false
            }

            Rectangle {
                id: buttonBack
                anchors.left: parent.left
                anchors.top: parent.top
                width: parent.width * 0.2
                height: parent.height
                color: "palegreen"
                Image {
                    id: imagePrevious
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width * 0.05
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height * 0.5
                    fillMode: Image.PreserveAspectFit
                    source: "images/previous.png"
                }
                Text {
                    anchors.left: imagePrevious.right
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("Back")
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        console.log("Back is pressed")
                        settingsHeader.makeInvisible()
                        if (settingsHeader.mainAreaObject !== null)
                            settingsHeader.mainAreaObject.destroy()
                    }
                }
            }

            Rectangle {
                anchors.left: buttonBack.right
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.bottom: buttonBack.bottom
                color: "ghostwhite"
                Text {
                    id: screenTitle
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: settingsHeader.pluginName
                }
            }
        }

        property int buttonOffset: width * 0.1
        property int buttonWidth: (width - buttonOffset * 4) / 3
        Rectangle {
            id: button0
            anchors.left: parent.left
            anchors.leftMargin: screenConnected.buttonOffset
            anchors.top: parent.top
            anchors.topMargin: (imageSettings.y - screenConnected.buttonWidth) / 2
            width: screenConnected.buttonWidth
            height: screenConnected.buttonWidth
            color: "transparent"
            border.color: "black"
            border.width: 1
            radius: 10
            property string pluginID: ""
            Image {
                id: pluginLogo
                anchors.fill: parent
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log("Button #0 is pressed")
                    if (button0.pluginID.length === 0) {
                        var uid = "1"
                        button0.pluginID = uid
                        return
                    }
                    var path = PluginsManager.getSettingsScreenPath(button0.pluginID)
                    var component = Qt.createComponent("file:///" + path);
                    if (component.status === Component.Ready) {
                        var settingsScreen = component.createObject(screenConnected);
                        settingsScreen.anchors.fill = screenConnected
                        settingsScreen.z = screenConnected.z + 1
                        settingsHeader.pluginName = PluginsManager.getTitle(button0.pluginID)
                        settingsHeader.mainAreaObject = settingsScreen
                        settingsHeader.makeVisible()
                    }
                }
            }
            onPluginIDChanged: {
                pluginLogo.source = "image://pluginLogo/" + pluginID
            }
        }
        Rectangle {
            id: button1
            anchors.left: button0.right
            anchors.leftMargin: button0.anchors.leftMargin
            anchors.top: button0.top
            width: button0.width
            height: button0.height
            color: button0.color
            border.color: button0.border.color
            border.width: button0.border.width
            radius: button0.radius
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log("Button #1 is pressed")
                }
            }
        }
        Rectangle {
            id: button2
            anchors.left: button1.right
            anchors.leftMargin: button0.anchors.leftMargin
            anchors.top: button0.top
            width: button0.width
            height: button0.height
            color: button0.color
            border.color: button0.border.color
            border.width: button0.border.width
            radius: button0.radius
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log("Button #2 is pressed")
                }
            }
        }
    }

    Component.onCompleted: {
    }
}
