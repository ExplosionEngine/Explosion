import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

ETextField {
    property double from: 0.0
    property double to: 1.0

    id: root
    implicitWidth: 100

    validator: DoubleValidator {
        bottom: root.from
        top: root.to
    }
}
