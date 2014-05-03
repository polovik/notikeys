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
            PropertyChanges { target: screenDisconnected; x: 0; z: 1}
            PropertyChanges { target: screenConnected; x: 0; z: 1}
        },
        State {
            name: "DISCONNECTED"
            PropertyChanges { target: screenInitialization; x: 0; z: 1}
            PropertyChanges { target: screenDisconnected; x: 0; z: 10}
            PropertyChanges { target: screenConnected; x: 0; z: 1}
        },
        State {
            name: "CONNECTED"
            extend: "DISCONNECTED"
        },
        State {
            name: "ACTIVE"
            PropertyChanges { target: screenInitialization; x: 0; z: 1}
            PropertyChanges { target: screenDisconnected; x: width ; z: 1}
            PropertyChanges { target: screenConnected; z: 10}
        }
    ]
    transitions: [
        Transition {
            from: "CONNECTED"
            to: "ACTIVE"
            SequentialAnimation {
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
        color: {
            if (mainScreen.state === "DISCONNECTED")
                return "#ff9807"
            else
                return "#41c445"
        }
        Image {
            id: imageDisconnected
            anchors.left: parent.left
            anchors.leftMargin: parent.width * 0.05
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width * 0.3
            fillMode: Image.PreserveAspectFit
            source: {
                if (mainScreen.state === "DISCONNECTED")
                    return "images/disconnect.png"
                else
                    return "images/connect.png"
            }
        }
        Text {
            anchors.left: imageDisconnected.right
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: {
                if (mainScreen.state === "DISCONNECTED")
                    return qsTr("Notikeys is disconnected.\nPlug it in USB port")
                else
                    return qsTr("Notikeys is connected!")
            }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (mainScreen.state === "DISCONNECTED")
                    mainScreen.state = "CONNECTED"
                else
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
                    button0.pluginID = "1"
                    console.log("Button #0 is pressed")
                    var path = PluginsManager.getSettingsScreenPath("1")
                    var component = Qt.createComponent("file:///" + path);
                    if (component.status === Component.Ready) {
                        var settingsScreen = component.createObject(mainScreen);
                        settingsScreen.anchors.fill = mainScreen
                        settingsScreen.z = 20
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
