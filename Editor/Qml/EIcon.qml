import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Effects

Item {
    property string src: ''
    property int srcSize: ETheme.iconFontSize

    id: 'root'
    implicitWidth: imageWidget.implicitWidth
    implicitHeight: imageWidget.implicitHeight

    Image {
        id: 'imageWidget'
        source: root.src
        sourceSize.width: root.srcSize
        sourceSize.height: root.srcSize
        layer.enabled: true
        layer.effect: MultiEffect {
            brightness: 1
            colorization: 1
            colorizationColor: ETheme.fontColor;
        }
    }
}
