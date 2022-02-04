import QtQuick 2.12
import QtQuick.Controls 2.10
import QtQuick.Window 2.12
// import QtQuick.VirtualKeyboard 2.4
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
    FontLoader { name: "cascadia"; source: "Cascadia.ttf"; }
    FontLoader { id: saxmono; source: "/saxmono.ttf"; }


    // FPS COUNTER
    Column {
        id: fpsCounter
        visible: false
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

        wrapMode: "NoWrap";
        // renderType: Text.NativeRendering;
        textFormat: Text.PlainText
        antialiasing: true;


        // connect text generator
        // ---------------------
        text: TextGenerator.text;
        // ---------------------

        Component.onCompleted: {
            settings.createNewSetting("display.color", "#11dd11")
            settings.createNewSetting("display.fontfamily", "Ubuntu Mono")
            settings.createNewSetting("display.fontpixelSize", 8)
            settings.createNewSetting("display.fontbold", false)
        }
        Connections {
            target: settings
            function onValueChanged(setting) {
                var [group, name] = setting.split(".");
                if (group !== "display") {
                    return;
                }

                if (name.startsWith("font")) {
                    display.font[name.replace("font", "")] = settings.value(setting);
                }
                else {
                    display[name] = settings.value(setting);
                }
            }
        }
    }

    Glow {
        id: glow
        z: 0
        anchors.fill: display
        source: display

        Component.onCompleted: {
            settings.createNewSetting("glow.visible", false);
            settings.createNewSetting("glow.color", "#00ff00");
            settings.createNewSetting("glow.radius", 5.5);
            settings.createNewSetting("glow.samples", 27)
            settings.createNewSetting("glow.opacity", 0.5)
        }
        Connections {
            target: settings
            function onValueChanged(setting) {
                var [group, name] = setting.split(".");
                if (group !== "glow") {
                    return;
                }
                glow[name] = settings.value(setting);
            }
        }
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


    Loader {
        id: settingsViewLoader
        active: false
        // the settings view should be created after all settings where created
        sourceComponent: SettingsView {
            height: parent.height
            width: 210
            edge: Qt.RightEdge
            //opacity: 0
            visible: true
        }
    }

    Connections {
        target: keyListener
        function onKeyPressed(key) {
            if (key === Qt.Key_F11) {
                window.visibility === Window.FullScreen ? window.showNormal() : window.showFullScreen();
                window.flags = window.flags === Qt.Window ? Qt.Window | Qt.FramelessWindowHint : Qt.Window;
            }
            if (key === Qt.Key_Left) {
                settingsViewLoader.active = true
            }
        }
    }
}
