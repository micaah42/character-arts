import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Item {
    z: 2112
    Rectangle {
        anchors.fill: parent
        color: "#426912"
        opacity: 0.85
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 5
        Repeater {
            id: settingsRepeater

            model: settings.keys
            delegate: Item {
                id: inputField
                Layout.fillWidth: true
                Layout.preferredHeight: 60 + firstInGroup * 5

                property string group: modelData.split(".")[0]
                property string name: modelData.split(".")[1]
                property alias text: field.text
                property bool firstInGroup: {
                    if (modelData === settings.keys[0]) {
                        return true
                    }

                    for (var i = 1; i < settings.keys.length; i++) {
                        if (settings.keys[i] === modelData) {
                            if (settings.keys[i - 1].split(".")[0] !== settings.keys[i].split(".")[0])
                                return true
                        }
                    }
                    return false;
                }

                TextField {
                    id: field
                    width: parent.width
                    anchors.bottom: parent.bottom
                    text: settings.value(modelData)
                    onTextChanged: {
                        settings.changeSetting(modelData, text);
                    }
                }

                // group label
                Label {
                    visible: inputField.firstInGroup
                    anchors.margins: 2
                    anchors.top: parent.top
                    anchors.left: parent.left
                    text: inputField.group
                    font.bold: true
                }
                Rectangle {
                    width: parent.width
                    anchors.top: parent
                    height: 1
                    color: "#adadad"
                }

                // name label
                Label {
                    anchors.margins: 2
                    anchors.top: parent.top
                    anchors.right: parent.right
                    text: inputField.name
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }

        Loader {
            id: connector
            active: false
            sourceComponent: Connections {
                target: settings
                function onValueChanged(setting) {
                    for (var i = 0; i < settings.keys.length; i++) {
                        if (settings.keys[i] === setting && settingsRepeater.itemAt(i)) {
                            settingsRepeater.itemAt(i).text = settings.value(setting);
                            break;
                        }
                    }
                }
            }
        }
        Component.onCompleted: {
            connector.active = true;
        }
    }
}
