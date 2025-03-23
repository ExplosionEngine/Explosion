import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

Item {
    property string placeHolderText: ''
    property int wrapMode: TextInput.NoWrap
    property string text: ''

    id: root
    implicitWidth: textField.implicitWidth
    implicitHeight: textField.implicitHeight

    TextField {
        id: textField
        width: root.width
        height: root.height
        implicitWidth: 500
        text: root.text
        rightPadding: 10
        color: ETheme.fontColor
        placeholderText: root.placeHolderText
        placeholderTextColor: ETheme.placeHolderFontColor
        font.pixelSize: ETheme.contentFontSize
        font.family: ETheme.fontFamily
        wrapMode: root.wrapMode

        background: Rectangle {
            radius: 5
            color: ETheme.primaryBgColor
        }
    }
}
