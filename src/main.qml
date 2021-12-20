import QtQuick 2.12
import QtQuick.Controls 2.10
import QtQuick.Window 2.12
import QtQuick.VirtualKeyboard 2.4

import "."
import com.text.generator 1.0

Window {
    id: window
    width: 840
    height: 480
    visible: true
    flags: Qt.FramelessWindowHint | Qt.Window
    title: qsTr("Character Arts")

    Component.onCompleted: {
        window.showFullScreen();
    }

    color: "black";

    Text {
        id: fpsCounter
        visible: settings.showFps
        anchors.top: parent.top
        anchors.right: parent.right
        text: TextGenerator.measuredFps.toFixed(1)
        color: "#00ff00"
    }

    FontLoader { id: cascadia; source: "qrc:/Cascadia.ttf"; }
    FontLoader { id: saxmono; source: "qrc:/saxmono.ttf"; }

    Text {
        x: -10; y: -10;
        width: window.width + 20
        height: window.height + 20
        color: "green";
        font.family: "Monospace";
        font.pixelSize: TextGenerator.fontSize;
        font.bold: true
        wrapMode: "NoWrap";
        renderType: Text.NativeRendering;
        textFormat: Text.PlainText
        antialiasing: false;
        clip: true

        text: TextGenerator.text;

        MouseArea {
            anchors.fill: parent;
            onClicked: settings.visible = !settings.visible;
        }
    }

    SettingsDialog {
        id: settings;
        visible: false;
    }
}
