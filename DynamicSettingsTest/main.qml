import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Text {
        id: glowingText
        anchors.centerIn: parent

        text: "-"
        Component.onCompleted: {
            settings.createNewSetting("text.text", "2112")
        }

        Connections {
            target: settings
            function onValueChanged(setting) {
                var [group, name] = setting.split(".");
                if (group === "text") {
                    glowingText[name] = settings.value(setting);
                }
            }
        }
    }

    Glow {
        id: glow
        source: glowingText
        anchors.fill: glowingText
        Component.onCompleted: {
            settings.createNewSetting("glow.radius", 5)
            settings.createNewSetting("glow.samples", 17)
            settings.createNewSetting("glow.color", "#00ff00")
        }
        Connections {
            target: settings
            function onValueChanged(setting) {
                var [group, name] = setting.split(".");
                if (group === "glow") {
                    glow[name] = settings.value(setting)
                }
            }
        }
    }

    SettingsView {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 5

        height: parent.height
        width: parent.width / 3
    }
}
