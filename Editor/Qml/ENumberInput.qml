import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

Item {
    property int value: spinBox.value
    property int from: spinBox.from
    property int to: spinBox.to
    property bool editable: spinBox.editable

    id: root
    implicitWidth: spinBox.implicitWidth
    implicitHeight: spinBox.implicitHeight

    SpinBox {
        id: spinBox
        implicitHeight: textField.implicitHeight
        value: root.value
        editable: root.editable
        from: root.from
        to: root.to

        contentItem: ETextField {
            id: textField
            implicitWidth: 40
            text: spinBox.textFromValue(spinBox.value)
            validator: spinBox.validator
        }

        down.indicator: Rectangle {
            id: downIndicator
            x: 0
            implicitWidth: 25
            implicitHeight: textField.implicitHeight
            radius: 5
            color: spinBox.down.hovered ? ETheme.secondaryBgColor : ETheme.primaryBgColor

            EIcon {
                name: 'minus'
                anchors.centerIn: downIndicator
            }
        }

        up.indicator: Rectangle {
            id: upIndicator
            x: spinBox.width - width
            implicitWidth: 25
            implicitHeight: textField.implicitHeight
            radius: 5
            color: spinBox.up.hovered ? ETheme.secondaryBgColor : ETheme.primaryBgColor

            EIcon {
                name: 'add'
                anchors.centerIn: upIndicator
            }
        }

        background: Rectangle {
            implicitWidth: 100
            radius: 5
            color: ETheme.primaryBgColor
        }
    }
}
