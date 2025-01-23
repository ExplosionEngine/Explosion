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
                    text: 'Icon Button 1'
                    iconSrc: 'Resource/Icon/home.svg'
                    onClicked: {
                        console.log('icon button clicked')
                    }
                }

                EButton {
                    text: 'Icon Button 2'
                    iconSrc: 'Resource/Icon/anchor.svg'
                    onClicked: {
                        console.log('icon button clicked')
                    }
                }

                EButton {
                    iconSrc: 'Resource/Icon/alarm.svg'
                    onClicked: {
                        console.log('icon button clicked')
                    }
                }

                EButton {
                    iconSrc: 'Resource/Icon/api.svg'
                    shape: EButton.Shape.Round
                    onClicked: {
                        console.log('icon button clicked')
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
                Layout.topMargin: 15

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

            RowLayout {
                Layout.leftMargin: 5
                Layout.topMargin: 15

                EText {
                    text: 'Icons'
                    style: EText.Style.Title1
                }
            }

            RowLayout {
                Layout.leftMargin: 5

                EIcon { src: 'Resource/Icon/arrow-left.svg' }
                EIcon { src: 'Resource/Icon/arrow-left-up.svg' }
                EIcon { src: 'Resource/Icon/arrow-up.svg' }
                EIcon { src: 'Resource/Icon/arrow-right-up.svg' }
                EIcon { src: 'Resource/Icon/arrow-right.svg' }
                EIcon { src: 'Resource/Icon/arrow-right-down.svg' }
                EIcon { src: 'Resource/Icon/arrow-down.svg' }
                EIcon { src: 'Resource/Icon/arrow-left-down.svg' }

                EIcon { src: 'Resource/Icon/arrow-left-circle.svg' }
                EIcon { src: 'Resource/Icon/arrow-left-up-circle.svg' }
                EIcon { src: 'Resource/Icon/arrow-up-circle.svg' }
                EIcon { src: 'Resource/Icon/arrow-right-up-circle.svg' }
                EIcon { src: 'Resource/Icon/arrow-right-circle.svg' }
                EIcon { src: 'Resource/Icon/arrow-right-down-circle.svg' }
                EIcon { src: 'Resource/Icon/arrow-down-circle.svg' }
                EIcon { src: 'Resource/Icon/arrow-left-down-circle.svg' }

                EIcon { src: 'Resource/Icon/caret-left.svg' }
                EIcon { src: 'Resource/Icon/caret-up.svg' }
                EIcon { src: 'Resource/Icon/caret-right.svg' }
                EIcon { src: 'Resource/Icon/caret-down.svg' }

                EIcon { src: 'Resource/Icon/chevron-left.svg' }
                EIcon { src: 'Resource/Icon/chevron-up.svg' }
                EIcon { src: 'Resource/Icon/chevron-right.svg' }
                EIcon { src: 'Resource/Icon/chevron-down.svg' }

                EIcon { src: 'Resource/Icon/chevron-left-double.svg' }
                EIcon { src: 'Resource/Icon/chevron-up-double.svg' }
                EIcon { src: 'Resource/Icon/chevron-right-double.svg' }
                EIcon { src: 'Resource/Icon/chevron-down-double.svg' }
            }

            RowLayout {
                Layout.leftMargin: 5

                EIcon { src: 'Resource/Icon/download.svg' }
                EIcon { src: 'Resource/Icon/login.svg' }
                EIcon { src: 'Resource/Icon/logout.svg' }
                EIcon { src: 'Resource/Icon/fullscreen-1.svg' }
                EIcon { src: 'Resource/Icon/fullscreen-exit-1.svg' }
                EIcon { src: 'Resource/Icon/load.svg' }
                EIcon { src: 'Resource/Icon/filter.svg' }
                EIcon { src: 'Resource/Icon/dashboard.svg' }
                EIcon { src: 'Resource/Icon/add-rectangle.svg' }
                EIcon { src: 'Resource/Icon/app.svg' }
                EIcon { src: 'Resource/Icon/hard-disk-storage.svg' }
                EIcon { src: 'Resource/Icon/share.svg' }
                EIcon { src: 'Resource/Icon/system-log.svg' }
                EIcon { src: 'Resource/Icon/system-code.svg' }
                EIcon { src: 'Resource/Icon/system-setting.svg' }
                EIcon { src: 'Resource/Icon/hourglass.svg' }
                EIcon { src: 'Resource/Icon/more.svg' }
                EIcon { src: 'Resource/Icon/service.svg' }
                EIcon { src: 'Resource/Icon/drag-move.svg' }
                EIcon { src: 'Resource/Icon/focus.svg' }
                EIcon { src: 'Resource/Icon/fill-color.svg' }
                EIcon { src: 'Resource/Icon/sip.svg' }
                EIcon { src: 'Resource/Icon/slice.svg' }
                EIcon { src: 'Resource/Icon/copy.svg' }
                EIcon { src: 'Resource/Icon/file-add.svg' }
                EIcon { src: 'Resource/Icon/folder-add.svg' }
                EIcon { src: 'Resource/Icon/folder.svg' }
                EIcon { src: 'Resource/Icon/folder-import.svg' }
            }
        }
    }
}
