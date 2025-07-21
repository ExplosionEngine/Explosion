import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

EFloatInput {
    property double step: 0.1
    property int decimals: 2
    property bool sliding: false
    property double slideStartValue: root.value
    property int slideStartX: 0

    id: root

    Rectangle {
        implicitWidth: (root.implicitWidth - 4) * (root.value - root.from) / (root.to - root.from)
        implicitHeight: 3
        radius: 5
        anchors.bottom: root.bottom
        anchors.left: root.left
        anchors.leftMargin: 2
        color: ETheme.primaryColor
    }

    MouseArea {
        implicitWidth: root.implicitWidth
        implicitHeight: root.implicitHeight / 2
        anchors.bottom: root.bottom
        hoverEnabled: true
        preventStealing: true

        onPressed: (mouse) => {
            root.sliding = true;
            root.slideStartX = mouse.x;
            root.slideStartValue = root.value;
        }

        onReleased: {
            root.sliding = false;
        }

        onPositionChanged: (mouse) => {
            if (root.sliding) {
                const distance = mouse.x - root.slideStartX;
                const valueDistance = (distance / 10.0) * root.step;
                root.value = Math.min(Math.max(root.slideStartValue + valueDistance, root.from), root.to).toFixed(root.decimals);
            }
        }
    }
}
