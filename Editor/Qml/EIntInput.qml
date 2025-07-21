import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

Item {
    property int value: Number(textField.text)
    property int from: 0
    property int to: 10

    id: root
    implicitWidth: textField.implicitWidth
    implicitHeight: textField.implicitHeight

    ETextField {
        id: textField
        implicitWidth: 100
        text: value

        onAccepted: {
            root.value = Number(displayText)
        }

        validator: IntValidator {
            bottom: root.from
            top: root.to
        }
    }
}
