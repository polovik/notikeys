import QtQuick 2.1

Item {
    id: settingsScreen
    width: 500
    height: 200

    enabled: true
    focus: true
    visible: true
    property string pluginName: "PluginName"
    property var mainAreaObject: null

    //  Absorb clicks outside the buttons
    MouseArea {
        anchors.fill: parent
        onClicked: {
        }
    }

    //  Move popupItem to top of Z stack. Grab focus and enable grabing mouse events
    function makeVisible(pluginId) {
        pluginName = PluginsManager.getTitle(pluginId)
        console.log("Open settings for plugin \"" + pluginName + "\". ID: " + pluginId)
        var path = PluginsManager.getSettingsScreenPath(pluginId)
        var component = Qt.createComponent("file:///" + path);
        if (component.status === Component.Ready) {
            var pluginSettings = component.createObject(settingsScreen);
            pluginSettings.anchors.fill = pluginsSettings
            pluginSettings.z = settingsScreen.z + 1
            mainAreaObject = pluginSettings
        }
        z = 100
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
        if (mainAreaObject !== null)
            mainAreaObject.destroy()
    }

    Item {
        id: settingsHeader
        anchors.fill: parent
        anchors.bottomMargin: parent.height * 0.8

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
                    settingsScreen.makeInvisible()
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
                text: settingsScreen.pluginName
            }
        }
    }

    Item {
        id: pluginsSettings
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: settingsHeader.bottom
        anchors.bottom: parent.bottom
    }
}
