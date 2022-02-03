import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

ColumnLayout {
    Connections {
        target: settings
        function onValueChanged(setting) {
            for (var i = 0; i < settings.keys.length; i++) {
                if (settings.keys[i] === setting) {
                    settingsRepeater.itemAt(i).text = settings.value(setting);
                    break;
                }
            }
        }
    }

    Repeater {
        id: settingsRepeater
        Layout.fillWidth: true

        model: settings.keys
        delegate: TextField {
            width: parent.width

            text: settings.value(modelData)
            onTextChanged: {
                settings.changeSetting(modelData, text);
            }

            Label {
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: 2

                text: modelData
            }
        }
    }

    Item {
        Layout.fillHeight: true
    }
}
