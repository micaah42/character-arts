import QtQuick 2.0
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0
import QtQuick.Window 2.12

import com.text.generator 1.0

Window {
    id: options;
    title: "Options";
    flags: Qt.Dialog;

    width: 750;
    height: 380;
    // border.color: "white";
    // border.width: 3;
    color: "#bdbcbd";
    // radius: 5;

    // capture mouse from background
    MouseArea {
        anchors.fill: parent
    }

    RowLayout {
        x: 5; y: 5;
        // width: parent.width / 3;

        // Chances
        Rectangle {
            implicitWidth: 200;
            implicitHeight: options.height - 10;
            color: "#aaaacd";
            radius: 5;
            border.color: "black";
            border.width: 2;

            ColumnLayout {
                id: chances
                width: parent.width - 6;
                x: 5; y: 5;

                Text {
                    text: "## Chances";
                    textFormat: Text.MarkdownText;
                }

                TriangleSelector {}

                // num resets
                LabeledTextField {
                    text: "Resets:"
                    startValue: TextGenerator.resetChance;
                    placeholderText:  "resets per frame, e.g. 21";
                    onInputChanged: {
                        TextGenerator.resetChance = input;
                    }
                }
            }

        }


        // Display Settings
        Rectangle {
            implicitWidth: 250;
            implicitHeight: options.height - 10;
            color: "#bbbbcd";
            radius: 5;
            border.color: "black";
            border.width: 2;

            ColumnLayout {
                id: display
                width: parent.width - 6;
                x: 5; y: 5;

                Text {
                    text: "## Display Settings";
                    textFormat: Text.MarkdownText;
                }

                // fps
                LabeledTextField {
                    text: "FPS:";
                    startValue: TextGenerator.fps;
                    placeholderText: "fps, e.g. 60";
                    onInputChanged: {
                        TextGenerator.fps = input;
                    }
                }

                // cols
                LabeledTextField {
                    text: "Columns:";
                    startValue: TextGenerator.cols;
                    placeholderText: "num cols, e.g. 420";
                    onInputChanged: {
                        TextGenerator.cols = input;
                    }
                }

                // rows
                LabeledTextField {
                    text: "Rows:";
                    startValue: TextGenerator.rows;
                    placeholderText: "num row, e.g. 69";
                    onInputChanged: {
                        TextGenerator.rows = input;
                    }
                }

                // fontsize
                LabeledTextField {
                    text: "Font Size:";
                    startValue: TextGenerator.fontSize;
                    placeholderText:  "font size in px, e.g. 12";
                    onInputChanged: {
                        TextGenerator.fontSize = input;
                    }
                }
            }
        }
    }
}
