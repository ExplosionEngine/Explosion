import QtQuick
import QtQuick.Layouts

Rectangle {
    RowLayout {
        anchors.fill: parent
        spacing: 0

        ColumnLayout {
            Layout.preferredWidth: 5
            Layout.fillWidth: true
            Layout.fillHeight: true

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: ETheme.primaryBgColor

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20

                    RowLayout {
                        Layout.preferredHeight: 1
                        Layout.fillHeight: true
                        Layout.fillWidth: true

                        Image {
                            source: Qt.url('Resource/Image/Logo.png')
                            sourceSize.width: 200
                        }
                    }

                    RowLayout {
                        Layout.preferredHeight: 7
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }
                }
            }
        }

        ColumnLayout {
            Layout.preferredWidth: 14
            Layout.fillWidth: true
            Layout.fillHeight: true

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: ETheme.bgColor
            }
        }
    }
}
