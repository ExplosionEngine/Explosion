import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Layouts


Item {

    enum Size {
        Small,
        Middle,
        Large
    }

    enum Filler {
        None,
        Withchar
    }

    property string text: ''
    property string icon: ''
    property bool disabled: false
    property bool checked: false
    property int size: ESwitch.Size.Middle
    property int filler: ESwitch.Filler.None

    signal clicked()

    id: 'root'
    implicitWidth: switchWidget.implicitWidth
    implicitHeight: switchWidget.implicitHeight

    Switch {
        id: 'switchWidget'
        enabled: !root.disabled
        checked: root.checked
        onClicked: root.clicked()
        width: root.width
        indicator: Rectangle {
            function getBackgroundColor(checked, disabled) {
                if (disabled) {
                    return checked ? ETheme.disabledCheckedColor : ETheme.disabledColor;
                }
                return checked ? ETheme.primaryColor : ETheme.inputBoxColor;
            }

            implicitWidth: 40 + 2 * (root.size - 1)
            implicitHeight: 24 + 2 * (root.size - 1)
            x: parent.leftPadding
            anchors.verticalCenter: parent.verticalCenter
            radius: width / 2
            color: getBackgroundColor(switchWidget.checked, root.disabled)

            Rectangle {
                function getX(check, down) {
                    if (check && down)
                        return parent.width - 2 * radius - 8;
                    else
                        return check ? parent.width - width - 2 : 2

                }

                id: handle
                x: getX(switchWidget.checked, switchWidget.down)
                anchors.verticalCenter: parent.verticalCenter
                radius: switchWidget.checked ? 9 + root.size : 7 + root.size
                height: 2 * radius
                width: switchWidget.down ? 2 * radius + 6 : 2 * radius
                color: ETheme.fontColor;

                Behavior on width {
                    NumberAnimation {
                        duration: 100
                    }
                }
                Behavior on x {
                    NumberAnimation {
                        duration: 100
                        easing.type: easing.InOutCubic
                    }
                }
            }

            Text {
                function getChar(filler, checked, down) {
                    if (filler == ESwitch.Filler.None || down) {
                        return '';
                    } else {
                        return checked ? "开" : "关";
                    }
                }

                id: switchText
                font.pixelSize: ETheme.contentFontSize
                font.family: ETheme.fontFamily
                color: ETheme.fontColor
                x: switchWidget.checked ? (parent.width - handle.radius * 2 - font.pixelSize - 2) / 2 : (parent.width + handle.radius * 2 - font.pixelSize + 2) / 2;
                anchors.verticalCenter: parent.verticalCenter
                text: getChar(root.filler, switchWidget.checked, switchWidget.down)
                Behavior on x {
                    NumberAnimation {
                        duration: 100
                        easing.type: easing.OutExpo
                    }
                }
            }
        }


        contentItem: Text {
            text: root.text
            font.pixelSize: ETheme.contentFontSize
            font.family: ETheme.fontFamily
            color: ETheme.fontColor
            verticalAlignment: Text.AlignVCenter
            leftPadding: switchWidget.indicator.width + 5
        }
    }
}
