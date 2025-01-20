import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

Button {
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
        color: parent.down ? ETheme.focusColor : ETheme.primaryColor
        radius: 10
    }
}
