import QtQuick 2.1
import QtQuick.Controls 1.0

Rectangle {
    id: pluginSettingsScreen
    width: 500 // define size of screen for debug and preview with qmlscene
    height: 200

    Rectangle {
        id: rectStatus
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.2
        height: parent.height * 0.3
        width: parent.width * 0.9
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

    AnimatedImage {
        id: imageVerifyInProgress
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: rectStatus.bottom
        anchors.topMargin: parent.height * 0.1
        anchors.bottom: parent.bottom
        anchors.bottomMargin: parent.height * 0.1
//        height: labelLogin.height
        fillMode: Image.PreserveAspectFit
        source: "checking.gif"
        visible: false
    }

    function displayChecking(status) {
        if (status === "start") {
            rectStatus.color = "#2a76a5"
            labelStatus.color = "white"
            labelStatus.text = qsTr("Checking presence of Thunderbird's addon \"Notikeys\"")
            imageVerifyInProgress.playing = true
            imageVerifyInProgress.visible = true
        } else if (status === "absent") {
            rectStatus.color = "#ff7171"
            labelStatus.color = "black"
            labelStatus.text = qsTr("Thunderbird's addon \"Notikeys\" isn't detected.\nVerify that Thunderbird is started and addon \"Notikeys\" is installed")
            imageVerifyInProgress.visible = false
            imageVerifyInProgress.playing = false
        } else {
            rectStatus.color = "#039103"
            labelStatus.color = "white"
            labelStatus.text = qsTr("Thunderbird is present. Messages count: ") + status
            imageVerifyInProgress.visible = false
            imageVerifyInProgress.playing = false
        }
    }

    function displayAbsent() {
        displayChecking("absent")
    }

    function displayEventsNumber(num) {
        displayChecking(num)
    }

    Component.onCompleted: {
        console.log("Start configuration of Thunderbird plugin")
        Thunderbird.addonIsAbsent.connect(displayAbsent)
        Thunderbird.messagesCount.connect(displayEventsNumber)
        displayChecking("start")
        Thunderbird.check()
    }

    //  NOTE: This slot may be called after starting active plugins, because deleting of component is delayed
    Component.onDestruction: {
        console.log("Finish configuration of Thunderbird plugin")
        Thunderbird.addonIsAbsent.disconnect(displayAbsent)
        Thunderbird.messagesCount.disconnect(displayEventsNumber)
    }
}
