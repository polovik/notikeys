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
        anchors.topMargin: parent.height * 0.3
        height: parent.height * 0.15
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

    Text {
        id: labelPollingInterval
        anchors.left: rectStatus.left
        anchors.top: rectStatus.bottom
        anchors.topMargin: parent.height * 0.3
        height: labelStatus.height
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        text: qsTr("Check Skype status every:")
    }

    SpinBox {
        id: spinBoxPollingInterval
        anchors.left: labelPollingInterval.right
        anchors.leftMargin: parent.width * 0.05
        anchors.verticalCenter: labelPollingInterval.verticalCenter
        minimumValue: 1
        maximumValue: 100
        stepSize: 1
        suffix: ""
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

            var interval = value
            if (pollPerSeconds === false)
                interval = interval * 60
            if ((Settings !== null) && (suffix.length > 0))
                Settings.set("Skype/pollingInterval", interval)
        }
    }

    function displayAbsent() {
        rectStatus.color = "#2a76a5"
        labelStatus.color = "white"
        labelStatus.text = qsTr("Skype isn't detected")
    }

    function displayEventsNumber(num) {
        rectStatus.color = "#039103"
        labelStatus.color = "white"
        labelStatus.text = qsTr("Skype is present. Events count: ") + num
    }

    Component.onCompleted: {
        console.log("Start configuration of Skype plugin")
        Skype.skypeIsAbsent.connect(displayAbsent)
        Skype.eventsCount.connect(displayEventsNumber)
        var strInterval = Settings.get("Skype/pollingInterval")
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
        Skype.check()
    }

    //  NOTE: This slot may be called after starting active plugins, because deleting of component is delayed
    Component.onDestruction: {
        console.log("Finish configuration of Skype plugin")
        Skype.skypeIsAbsent.disconnect(displayAbsent)
        Skype.eventsCount.disconnect(displayEventsNumber)
    }
}
