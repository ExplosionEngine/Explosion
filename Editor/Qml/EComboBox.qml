import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

ComboBox {
    id: root
    model: model

    implicitWidth: 100
    implicitHeight: 30

    indicator: Canvas {
        id: canvas
        x: root.width - width - root.rightPadding
        y: root.topPadding + (root.availableHeight - height) / 2
        width: 10
        height: 6
        contextType: "2d"

        Connections {
            target: root
            function onPressedChanged() { canvas.requestPaint(); }
        }

        onPaint: {
            context.reset();
            context.moveTo(0, 0);
            context.lineTo(width, 0);
            context.lineTo(width / 2, height);
            context.closePath();
            context.fillStyle = ETheme.fontColor
            context.fill();
        }
    }

    contentItem: Text {
        leftPadding: 10
        rightPadding: root.indicator.width + root.spacing

        text: root.displayText
        font.pixelSize: ETheme.contentFontSize
        font.family: ETheme.fontFamily
        color: ETheme.fontColor
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        width: root.width
        height: root.height
        radius: 5
        color: ETheme.primaryBgColor
    }

    popup: Popup {
        y: root.height - 1
        width: root.width
        height: Math.min(contentItem.implicitHeight, root.Window.height - topMargin - bottomMargin)
        padding: 1

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: root.popup.visible ? root.delegateModel : null
            currentIndex: root.highlightedIndex

            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            radius: 5
            color: ETheme.primaryBgColor
        }
    }
}
