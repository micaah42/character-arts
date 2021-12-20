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
        anchors.top: parent.top
        anchors.right: parent.right
        text: TextGenerator.measuredFps.toFixed(1)
        color: "#00ff00"
    }

    FontLoader { id: cascadia; source: "./Cascadia.ttf"; }
    FontLoader { id: saxmono; source: "./saxmono.ttf"; }

    Text {
        x: -10; y: -10;
        width: window.width + 20
        height: window.height + 20
        color: "white";
        font.family: "Monospace";
        font.pixelSize: TextGenerator.fontSize;
        wrapMode: "NoWrap";
        // renderType: Text.NativeRendering;
        textFormat: Text.PlainText
        antialiasing: false;

        text: TextGenerator.text;

        MouseArea {
            anchors.fill: parent;
            onClicked: options.visible = !options.visible;
        }
    }

    SettingsDialog {
        id: options;
        visible: false;
    }
}
