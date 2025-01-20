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
                    text: 'TODO Secondary Button'
                }

                EButton {
                    text: 'TODO Disabled Button'
                }

                EButton {
                    text: 'TODO Icon Button'
                }

                EButton {
                    text: 'TODO Outline Button'
                }
            }

            RowLayout {
                Layout.margins: 5

                EButton {
                    text: 'TODO Dashed Button'
                }

                EButton {
                    text: 'TODO Dashed Button'
                }

                EButton {
                    text: 'TODO Text Button'
                }
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
            }
        }
    }
}
