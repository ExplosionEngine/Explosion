import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    color: ETheme.bgColor

    ScrollView {
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10

            RowLayout {
                Layout.leftMargin: 5

                // TODO replace with EText
                Text {
                    text: 'Buttons'
                    font.bold: true
                    font.pixelSize: 18
                    color: ETheme.fontColor
                }
            }

            RowLayout {
                Layout.margins: 5

                EButton {
                    text: 'Basic'
                }
            }
        }
    }
}
