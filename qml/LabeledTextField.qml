import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.12

RowLayout {
    id: layout;
    spacing: 0;
    width: parent.width
    property string text: "";
    property string input: "";
    property string startValue: "";
    property string placeholderText: "";
    property var validator: IntValidator {bottom: 1; top: 1000000};

    Text {
        Layout.preferredWidth:  1 * layout.width / 3;
        text: layout.text;
    }

    TextField {
        id: field
        text: layout.startValue;
        Layout.preferredWidth: 2 * layout.width / 3 - 3;
        placeholderText: layout.placeholderText;
        onTextEdited: if(acceptableInput) layout.input = text;
        validator: layout.validator;
    }
}
