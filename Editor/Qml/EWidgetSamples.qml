import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    color: ETheme.bgColor

    ScrollView {
        id: 'scrollview'
        anchors.fill: parent
        anchors.margins: 20

        ColumnLayout {
            width: Math.max(implicitWidth, scrollview.availableWidth)

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
                    text: 'Basic Button'
                    onClicked: {
                        console.log('basic button clicked')
                    }
                }

                EButton {
                    style: EButton.Style.Secondary
                    text: 'Secondary Button'
                    onClicked: {
                        console.log('secondary button clicked')
                    }
                }

                EButton {
                    text: 'Disabled Button'
                    disabled: true
                    onClicked: {
                        console.log('disabled button clicked')
                    }
                }
            }

            RowLayout {
                Layout.margins: 5

                EButton {
                    text: 'Large Button'
                    size: EButton.Size.Large
                    onClicked: {
                        console.log('large button clicked')
                    }
                }

                EButton {
                    text: 'Middle Button'
                    size: EButton.Size.Middle
                    onClicked: {
                        console.log('middle button clicked')
                    }
                }

                EButton {
                    text: 'Small Button'
                    size: EButton.Size.Small
                    onClicked: {
                        console.log('small button clicked')
                    }
                }
            }

            RowLayout {
                Layout.margins: 5

                EButton {
                    text: 'Rect Button'
                    shape: EButton.Shape.Rect
                    onClicked: {
                        console.log('rect button clicked')
                    }
                }

                EButton {
                    text: 'Round Button'
                    shape: EButton.Shape.Round
                    onClicked: {
                        console.log('round button clicked')
                    }
                }
            }

            RowLayout {
                Layout.margins: 5

                EButton {
                    text: 'Block Button'
                    Layout.fillWidth: true
                    onClicked: {
                        console.log('block button clicked')
                    }
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
            }

            RowLayout {
                Layout.leftMargin: 5

                EText {
                    text: 'Basic Content'
                    style: EText.Style.Content
                }

                EText {
                    text: 'Sample Link'
                    href: 'https://github.com/ExplosionEngine/Explosion'
                    style: EText.Style.Link
                }

                EText {
                    text: 'Italic Content'
                    style: EText.Style.Italic
                }
            }
        }
    }
}
