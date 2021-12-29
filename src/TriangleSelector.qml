import QtQuick 2.0
import QtQuick.Shapes 1.12
import QtGraphicalEffects 1.12
import QtQuick.Layouts 1.12
// import com.text.generator 1.0

// transition chances
ColumnLayout {
    Shape {
        Item {
            id: tri
            property int d: 180;
            property int h: Math.sqrt(3 / 4) * this.d

            property int x1: 10;
            property int y1: 10 + h;

            property int x2: 10 + d / 2;
            property int y2: 10;

            property int x3: 10 + d;
            property int y3: 10 + h;
        }

        id: triangle
        height: 20 + tri.h;
        width: 200;
        ShapePath {
            strokeWidth: 5
            strokeColor: "black"

            strokeStyle: ShapePath.SolidLine
            startX: tri.x1; startY: tri.y1;
            PathLine { x: tri.x2; y: tri.y2; }
            PathLine { x: tri.x3; y: tri.y3; }
            PathLine { x: tri.x1; y: tri.y1; }


        }

        RadialGradient {
            anchors.fill: parent
            source: triangle
            verticalOffset: +(width/2 - (5 + tri.h/2))
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#002400" }
                GradientStop { position: 0.1; color: "black" }
            }
        }

        MouseArea {
            id: stepSelector
            anchors.fill: parent

            property real up: 1/3;
            property real equal: 1/3;
            property real down: 1/3;

            onPositionChanged: {
                up = (2*((Math.sqrt(3)*tri.d*tri.d)/4 + 5*Math.sqrt(3)*tri.d - 5*tri.d))/(Math.sqrt(3)*tri.d*tri.d) - mouseX/tri.d + mouseY/(Math.sqrt(3)*tri.d);
                equal = (2*((Math.sqrt(3)*tri.d*tri.d)/2 + 10*tri.d))/(Math.sqrt(3)*tri.d*tri.d) - (2*mouseY)/(Math.sqrt(3)*tri.d);
                down = (2*(-1/4*Math.sqrt(3)*tri.d*tri.d - 5*Math.sqrt(3)*tri.d - 5*tri.d))/(Math.sqrt(3)*tri.d*tri.d) + mouseX/tri.d + mouseY/(Math.sqrt(3)*tri.d);

                var sum;
                if (up < 0) {
                    sum = equal + down;
                    up = 0;
                    equal /= sum;
                    down /= sum;
                }
                if (equal < 0) {
                    sum = up + down;
                    up /= sum;
                    equal = 0;
                    down /= sum;
                }
                if (down < 0) {
                    sum = up + equal;
                    up /= sum;
                    equal /= sum;
                    down = 0;
                }

                Settings.stepChances = [up, equal, down]
            }

            // circle
            Rectangle {
                width: 5; height: 5;
                x: stepSelector.up * tri.x1 + stepSelector.equal * tri.x2 + stepSelector.down * tri.x3 - width/2;
                y: stepSelector.up * tri.y1 + stepSelector.equal * tri.y2 + stepSelector.down * tri.y3 - height/2;
                radius: 5;
                border.width: 5;
                border.color: "red";
            }
        }
    }

    Text {
        id: stepView
        elide: Text.ElideMiddle
        text:  {
            var setsss = settings;
            var stepChances = Settings.stepChances;
            var string = (100 * stepChances[0]).toFixed(0) + " (down), "
            + (100 * (stepChances[1] - stepChance[0])).toFixed(0) + " (equal), "
            + (100 * (1 - stepChances[1])).toFixed(0) + " (up)";
            return string;
        }
    }
}
