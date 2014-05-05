import QtQuick 2.1
import QtQuick.Controls 1.0

Rectangle {
    id: accountSettingsScreen
    width: 500 // define size of screen for debug and preview with qmlscene
    height: 200
    anchors.topMargin: parent.height * 0.2 // Make offset on top for display header

    Text {
        id: labelLogin
        anchors.left: parent.left
        anchors.leftMargin: parent.width * 0.05
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.1
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
        anchors.topMargin: parent.height * 0.1
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
        echoMode: TextInput.Password
    }

    Text {
        id: labelPollingInterval
        anchors.left: labelLogin.left
        anchors.top: labelPassword.bottom
        anchors.topMargin: parent.height * 0.1
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
        var account = Settings.get("GmailAtom/account")
        fieldLogin.text = account
        var strInterval = Settings.get("GmailAtom/pollingInterval")
        if (strInterval.length === 0)
            strInterval = 60
        var interval = parseInt(strInterval)
        if (interval > 99) {
            spinBoxPollingInterval.pollPerSeconds = false
            spinBoxPollingInterval.value = interval / 60
            spinBoxPollingInterval.suffix = qsTr("min")
        } else {
            spinBoxPollingInterval.pollPerSeconds = true
            spinBoxPollingInterval.value = interval
            spinBoxPollingInterval.suffix = qsTr("sec")
        }
    }

    Component.onDestruction: {
        console.log("Finish configuration of GMailAtom plugin")
        if ((Settings !== null) && (fieldLogin.text !== null))
            Settings.set("GmailAtom/account", fieldLogin.text)
        if ((Settings !== null) && (fieldPassword.text !== null) && (fieldPassword.text.length > 0))
            Settings.set("GmailAtom/password", fieldPassword.text)
        var interval = spinBoxPollingInterval.value
        if (spinBoxPollingInterval.pollPerSeconds === false)
            interval = interval * 60
        if (Settings !== null)
            Settings.set("GmailAtom/pollingInterval", interval)
    }
}
