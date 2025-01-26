import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Effects

Item {
    property string name: ''
    property int size: ETheme.iconFontSize

    id: 'root'
    implicitWidth: imageWidget.implicitWidth
    implicitHeight: imageWidget.implicitHeight

    Image {
        id: 'imageWidget'
        source: root.name === '' ? '' : 'Resource/Icon/%1.svg'.arg(root.name)
        sourceSize.width: root.size
        sourceSize.height: root.size
        layer.enabled: true
        layer.effect: MultiEffect {
            brightness: 1
            colorization: 1
            colorizationColor: ETheme.fontColor;
        }
    }
}
