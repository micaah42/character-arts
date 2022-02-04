import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Drawer {
    id: drawer
    z: 2112
    dim: false

    Connections {
        target: keyListener
        function onKeyPressed(key) {
            if (key === Qt.Key_Left)
                drawer.open();
            else if (key === Qt.Key_Escape)
                drawer.close();
        }
    }

    background: Rectangle {
        id: settingsBackground
        anchors.fill: parent
        color: "#426912"
        opacity: 0.85
        Connections {
            target: settings
            function onValueChanged(setting) {
                if (setting === "display.color") {
                    var color = settings.value(setting)
                    settingsBackground.color = Qt.lighter(color, 1.2)
                }
            }
        }
    }

    contentItem: Flickable {
        contentHeight: column.height + 50
        ColumnLayout {
            id: column
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
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
                        text: settings.value(modelData).toString()
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
                        //width: parent.width
                        anchors.top: parent.top
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

            Column {
                Layout.preferredHeight: 80
                spacing: 3
                Label {
                    text: "save & load"
                    font.bold: true
                }

                TextField {
                    id: fileField
                    width: parent.width
                    text: "default"
                }
                Row {
                    spacing: 2
                    Button {
                        text: "Save"
                        onClicked: {
                            settings.saveSettings(fileField.text);
                        }
                    }
                    Button {
                        text: "Load"
                        onClicked: {
                            settings.loadSettings(fileField.text);
                        }
                    }
                }
            }
        }

        Connections {
            target: settings
            function onValueChanged(setting) {
                for (var i = 0; i < settings.keys.length; i++) {
                    if (settings.keys[i] === setting && settingsRepeater.itemAt(i)) {
                        settingsRepeater.itemAt(i).text = settings.value(setting).toString();
                        break;
                    }
                }
            }
        }
    }
}
