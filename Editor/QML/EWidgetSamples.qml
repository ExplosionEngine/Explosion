import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    color: ETheme.bgColor

    ScrollView {
        width: parent.width
        height: parent.height

        ColumnLayout {
            width: parent.width

            RowLayout {
                width: parent.width
                height: 50

                Rectangle {
                    anchors.fill: parent
                    color: 'white'
                }
            }

            RowLayout {
                width: parent.width
                height: 50

                Rectangle {
                    anchors.fill: parent
                    color: 'white'
                }
            }
        }
    }
}
