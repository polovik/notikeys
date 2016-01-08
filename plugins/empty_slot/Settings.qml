import QtQuick 2.1

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
        color: "#039103"
        radius: 10
    }

    Text {
        id: labelStatus
        anchors.fill: rectStatus
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: "white"
        text: "Pure empty button"
    }

    Component.onCompleted: {
        console.log("Start configuration of EmptySlot plugin")
    }

    //  NOTE: This slot may be called after starting active plugins, because deleting of component is delayed
    Component.onDestruction: {
        console.log("Finish configuration of EmptySlot plugin")
    }
}
