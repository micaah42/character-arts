import QtQuick 2.12
import QtQuick.Controls 2.10
import QtQuick.Window 2.12
import QtQuick.VirtualKeyboard 2.4
import QtGraphicalEffects 1.15

import "."
// import com.text.generator 1.0

Window {
    id: window
    minimumWidth: 840
    minimumHeight: 480

    visible: true
    flags: Qt.Window
    title: qsTr("Character Arts")
    color: "black";

    // FONT LOADING
    FontLoader { id: cascadia; source: "qrc:/Cascadia.ttf"; }
    FontLoader { id: saxmono; source: "qrc:/saxmono.ttf"; }


    // FPS COUNTER
    Column {
        id: fpsCounter
        visible: settings.showFps
        z: 100

        anchors.top: parent.top
        anchors.right: parent.right

        Text {
            text: TextGenerator.measuredFps.toFixed(1)
            color: "#00ff00"
        }

        Text {
            text: fps.toFixed(1)
            color: "#00ff00"
        }
    }


    // CHARACTER DISPLAY
    Text {
        id: display
        x: -10; y: -10; z: 1;
        width: window.width + 20
        height: window.height + 20
        color: "green";
        font.family: "Monospace";
        font.pixelSize: Settings.fontSize;
        font.bold: true
        wrapMode: "NoWrap";
        renderType: Text.NativeRendering;
        textFormat: Text.PlainText
        antialiasing: false;
        focus: true

        text: TextGenerator.text;

        Keys.onPressed: {
            // TODO: feed key event to cpp singleton
            if (event.key === Qt.Key_F11) {
                window.visibility === Window.FullScreen ? window.showNormal() : window.showFullScreen();
                window.flags = window.flags === Qt.Window ? Qt.Window | Qt.FramelessWindowHint : Qt.Window;
            }
            else if (event.key === Qt.Key_Escape) {
                settings.visible = !settings.visible;
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (settings.visible)
                    settings.visible = false;
            }
        }
    }

    Glow {
        z: 0
        anchors.fill: display
        opacity: 0.8
        source: display
        color: "#0000aa"
        radius: 5.5
        samples: 17
    }

    // SETTINGS DIALOG
    SettingsDialog {
        id: settings;
        visible: false;
    }

    // MEASURING FPS
    property real fps: 0
    property real lastSwap: 0
    onFrameSwapped: {
        var now = new Date().getTime();
        var timeDiff = now - lastSwap;
        var newFps = timeDiff ? 1000. / timeDiff : 0;

        fps = 0.1 * newFps + 0.9 * fps;
        lastSwap = now;
    }
}
