import QtQuick 2.1
import QtQuick.Controls 1.0

Rectangle {
    id: accountSettingsScreen
    width: 500 // define size of screen for debug and preview with qmlscene
    height: 200
    anchors.topMargin: parent.height * 0.2 // Make offset on top for display header

    state: "INIT"
    states: [
        State {
            name: "INIT"
            PropertyChanges { target: rectStatus; color: "brown" }
            PropertyChanges { target: labelStatus; color: "white"; text: qsTr("Feel account's data") }
        },
        State {
            name: "ADDRESS_INVALID"
            PropertyChanges { target: rectStatus; color: "red" }
            PropertyChanges { target: labelStatus; color: "white"; text: qsTr("Format of e-mail should be: username@domain") }
        },
        State {
            name: "ACCOUNT_UNKNOWN"
            PropertyChanges { target: rectStatus; color: "yellow" }
            PropertyChanges { target: labelStatus; color: "white"; text: qsTr("Unknown account: verify account's data") }
        },
        State {
            name: "ACCOUNT_VALID"
            PropertyChanges { target: rectStatus; color: "green" }
            PropertyChanges { target: labelStatus; color: "white"; text: qsTr("Valid") }
        }
    ]

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
        focus: true
        // http://davidcel.is/blog/2012/09/06/stop-validating-email-addresses-with-regex/
        // Format of e-mail should be: username@domain
        validator: RegExpValidator { regExp: /.+@.+\..+/i }
        onTextChanged: {
            if (acceptableInput) {
                console.log("format is correct: " + text)
                if (fieldPassword.text.length > 0)
                    GmailAtom.verifyAccount(text, fieldPassword.text)
            } else {
                accountSettingsScreen.state = "ADDRESS_INVALID"
            }
        }
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
        onTextChanged: {
            if (fieldLogin.acceptableInput)
                GmailAtom.verifyAccount(fieldLogin.text, text)
        }
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

    Rectangle {
        id: rectStatus
        anchors.left: parent.left
        anchors.leftMargin: parent.width * 0.5
        anchors.right: fieldPassword.right
        anchors.verticalCenter: labelPollingInterval.verticalCenter
        height: labelLogin.height
        border.color: "black"
        border.width: 1
        radius: 10
    }

    Text {
        id: labelStatus
        anchors.fill: rectStatus
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    function displayAccountStatus(valid, error) {
        if (valid)
            accountSettingsScreen.state = "ACCOUNT_VALID"
        else
            accountSettingsScreen.state = "ACCOUNT_UNKNOWN"
    }

    Component.onCompleted: {
        console.log("Start configuration of GMailAtom plugin")
        GmailAtom.accountStatus.connect(displayAccountStatus)
        var account = Settings.get("GmailAtom/account")
        fieldLogin.text = account
        var psw = Settings.get("GmailAtom/password")
        fieldPassword.text = psw
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
        GmailAtom.accountStatus.disconnect(displayAccountStatus)
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
