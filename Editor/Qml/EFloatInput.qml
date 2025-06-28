import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

Item {
    property double value: 0.0
    property double from: 0.0
    property double to: 1.0

    id: root
    implicitWidth: textField.implicitWidth
    implicitHeight: textField.implicitHeight

    ETextField {
        id: textField
        implicitWidth: 100
        text: value

        onAccepted: {
            root.value = Number(text)
        }

        validator: DoubleValidator {
            bottom: root.from
            top: root.to
        }
    }
}
