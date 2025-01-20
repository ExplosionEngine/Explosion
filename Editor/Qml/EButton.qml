import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

Button {
    enum Style {
        Primary,
        Secondary,
        Disabled,
        Outline,
        Dashed
    }

    function getBackgroundColor(style, focus)
    {
        if (style === EButton.Style.Secondary) {
            return focus ? ETheme.secondaryFocusColor : ETheme.secondaryColor;
        } else if (style === EButton.Style.Disabled) {
            return ETheme.disabledColor;
        }
        // TODO more
        return focus ? ETheme.primaryFocusColor : ETheme.primaryColor;
    }

    property int style: EButton.Style.Primary

    // TODO disable onclick when disabled

    contentItem: Text {
        text: parent.text
        font.pixelSize: ETheme.contentFontSize
        font.family: ETheme.fontFamily
        color: ETheme.fontColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideNone
    }

    background: Rectangle {
        implicitWidth: 50
        color: getBackgroundColor(style, parent.down)
        radius: 10
    }
}
