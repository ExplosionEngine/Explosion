import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Layouts
import QtQuick.Effects

Item {
    enum Style {
        Primary,
        Secondary
    }

    enum Size {
        Small,
        Middle,
        Large
    }

    enum Shape {
        Rect,
        Round
    }

    property string text: ''
    property string iconSrc: ''
    property bool disabled: false
    property int style: EButton.Style.Primary
    property int size: EButton.Size.Middle
    property int shape: EButton.Shape.Rect
    signal clicked()

    id: 'root'
    implicitWidth: btnWidget.implicitWidth
    implicitHeight: btnWidget.implicitHeight

    Button {
        id: 'btnWidget'
        enabled: !root.disabled
        onClicked: root.clicked()
        leftPadding: 10
        rightPadding: 10
        topPadding: 5 + 2 * (root.size - 1)
        bottomPadding: 5 + 2 * (root.size - 1)
        width: root.width

        contentItem: RowLayout {
            spacing: 6

            EIcon {
                src: root.iconSrc
                visible: iconSrc !== ''
            }

            Text {
                Layout.fillWidth: true
                text: root.text
                font.pixelSize: ETheme.contentFontSize
                font.family: ETheme.fontFamily
                color: ETheme.fontColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideNone
                visible: root.text !== ''
            }
        }

        background: Rectangle {
            function getBackgroundColor(style, focus, hovered, disabled)
            {
                if (disabled) {
                    return ETheme.disabledColor;
                } else if (style === EButton.Style.Secondary) {
                    return focus ? ETheme.secondaryFocusColor : (hovered ? ETheme.secondaryHoverColor : ETheme.secondaryColor);
                }
                return focus ? ETheme.primaryFocusColor : (hovered ? ETheme.primaryHoverColor : ETheme.primaryColor);
            }

            color: getBackgroundColor(root.style, parent.down, parent.hovered, root.disabled)
            radius: 5 + root.shape * 8
        }
    }
}
