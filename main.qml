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
    title: qsTr("Letters")

    color: "black";

    FontLoader { id: cascadia; source: "./Cascadia.ttf"; }
    FontLoader { id: saxmono; source: "./saxmono.ttf"; }

    Text {
        x: -10; y: -10;
        width: window.width + 20
        height: window.height + 20
        color: "white";
        font.family: "saxmono";
        font.pixelSize: TextGenerator.fontSize;
        wrapMode: "NoWrap";

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
