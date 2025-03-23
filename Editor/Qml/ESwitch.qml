import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

Item {
    enum Size {
        Small,
        Middle,
        Large
    }

    enum Filler {
        None,
        Text
    }

    property string text: ''
    property string icon: ''
    property bool disabled: false
    property bool checked: false
    property int size: ESwitch.Size.Middle
    property int filler: ESwitch.Filler.None

    signal clicked()

    id: root
    implicitWidth: switchWidget.implicitWidth
    implicitHeight: switchWidget.implicitHeight

    Switch {
        id: switchWidget
        enabled: !root.disabled
        checked: root.checked
        onClicked: root.clicked()
        width: root.width
        indicator: Rectangle {
            function getBackgroundColor(checked, disabled) {
                if (disabled) {
                    return checked ? ETheme.primaryDisabledColor : ETheme.disabledColor;
                }
                return checked ? ETheme.primaryColor : ETheme.secondaryBgColor;
            }

            implicitWidth: 45 + 2 * (root.size - 1)
            implicitHeight: 24 + 2 * (root.size - 1)
            x: switchWidget.leftPadding
            anchors.verticalCenter: parent.verticalCenter
            radius: width / 4
            color: getBackgroundColor(switchWidget.checked, root.disabled)

            Rectangle {
                function getX(check, down) {
                    if (check && down)
                        return parent.width - 4 * radius - 8;
                    else
                        return check ? parent.width - width - 4 : 4
                }

                id: handle
                x: getX(switchWidget.checked, switchWidget.down)
                anchors.verticalCenter: parent.verticalCenter
                radius: 7 + root.size
                height: 2 * radius
                width: 2 * radius
                color: ETheme.fontColor;

                Behavior on width {
                    NumberAnimation {
                        duration: 100
                    }
                }
                Behavior on x {
                    NumberAnimation {
                        duration: 100
                    }
                }
            }

            Text {
                function getText(filler, checked, down) {
                    if (filler === ESwitch.Filler.None || down) {
                        return '';
                    } else {
                        return checked ? "Y" : "N";
                    }
                }

                id: switchText
                font.pixelSize: ETheme.contentFontSize
                font.family: ETheme.fontFamily
                color: ETheme.fontColor
                x: switchWidget.checked ? (parent.width - handle.radius * 2 - font.pixelSize - 2) / 2 : (parent.width + handle.radius * 2 - font.pixelSize + 2) / 2;
                anchors.verticalCenter: parent.verticalCenter
                text: getText(root.filler, switchWidget.checked, switchWidget.down)
                Behavior on x {
                    NumberAnimation {
                        duration: 100
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
