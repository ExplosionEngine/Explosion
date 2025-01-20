import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    color: ETheme.bgColor

    ScrollView {
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20

            RowLayout {
                Layout.leftMargin: 5

                EText {
                    text: 'Buttons'
                    style: EText.Style.Title1
                }
            }

            RowLayout {
                Layout.margins: 5

                EButton {
                    text: 'Basic'
                }

                EButton {
                    style: EButton.Style.Secondary
                    text: 'Secondary Button'
                }

                EButton {
                    style: EButton.Style.Disabled
                    text: 'Disabled Button'
                }

                // TODO icon button
            }

            RowLayout {
                Layout.leftMargin: 5
                Layout.topMargin: 10

                EText {
                    text: 'Texts'
                    style: EText.Style.Title1
                }
            }

            RowLayout {
                Layout.leftMargin: 5

                EText {
                    text: 'Title1'
                    style: EText.Style.Title1
                }

                EText {
                    text: 'Title2'
                    style: EText.Style.Title2
                }

                EText {
                    text: 'Title3'
                    style: EText.Style.Title3
                }

                EText {
                    text: 'Content'
                    style: EText.Style.Content
                }

                // TODO superlink text
                // TODO button text
            }
        }
    }
}
