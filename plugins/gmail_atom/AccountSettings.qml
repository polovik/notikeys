import QtQuick 2.1
import QtQuick.Controls 1.0

Rectangle {
    id: accountSettingsScreen
    width: 500
    height: 200
    anchors.topMargin: parent.height * 0.1

    Rectangle {
        id: buttonBack
        anchors.left: parent.left
        anchors.top: parent.top
        width: parent.width * 0.2
        height: parent.height * 0.2
        color: "palegreen"
        Image {
            id: imagePrevious
            anchors.left: parent.left
            anchors.leftMargin: parent.width * 0.05
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height * 0.5
            fillMode: Image.PreserveAspectFit
//            source: "previous.png"
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
            }
        }
    }

    Text {
        id: screenTitle
        anchors.left: buttonBack.right
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: buttonBack.bottom
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        text: qsTr("Gmail account")
    }

    Text {
        id: labelLogin
        anchors.left: parent.left
        anchors.leftMargin: parent.width * 0.05
        anchors.top: buttonBack.bottom
        anchors.topMargin: parent.height * 0.05
        width: parent.width * 0.15
        height: parent.height * 0.2
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        text: qsTr("Login:")
    }

    TextField {
        id: fieldLogin
        anchors.left: labelLogin.right
        anchors.leftMargin: parent.width * 0.05
        anchors.right: parent.right
        anchors.rightMargin: parent.width * 0.05
        anchors.verticalCenter: labelLogin.verticalCenter
        height: parent.height * 0.15
    }

    Text {
        id: labelPassword
        anchors.left: labelLogin.left
        anchors.top: labelLogin.bottom
        anchors.topMargin: parent.height * 0.05
        width: labelLogin.width
        height: labelLogin.height
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        text: qsTr("Password:")
    }

    TextField {
        id: fieldPassword
        anchors.left: fieldLogin.left
        anchors.verticalCenter: labelPassword.verticalCenter
        width: fieldLogin.width
        height: fieldLogin.height
        echoMode: TextInput.PasswordEchoOnEdit
    }

    Text {
        id: labelPollingInterval
        anchors.left: labelLogin.left
        anchors.top: labelPassword.bottom
        anchors.topMargin: parent.height * 0.05
        height: labelLogin.height
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        text: qsTr("Check for new mails every:")
    }

    SpinBox {
        id: spinBoxPollingInterval
        anchors.left: labelPollingInterval.right
        anchors.leftMargin: parent.width * 0.05
        anchors.verticalCenter: labelPollingInterval.verticalCenter
        minimumValue: 1
        maximumValue: 100
        stepSize: 1
        suffix: qsTr("sec")
        property bool pollPerSeconds: true
        onValueChanged: {
            if (pollPerSeconds && (value > 99)) {
                pollPerSeconds = false
                value = 2
                suffix = qsTr("min")
            } else if (!pollPerSeconds && (value < 2)) {
                pollPerSeconds = true
                value = 99
                suffix = qsTr("sec")
            }
        }
    }

    Component.onCompleted: {
        console.log("Start configuration of GMailAtom plugin")
    }

    Component.onDestruction: {
        console.log("Finish configuration of GMailAtom plugin")
    }
}
