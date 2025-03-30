import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Layouts

Rectangle {
    color: ETheme.bgColor

    ScrollView {
        id: scrollview
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
                    icon: 'home'
                    onClicked: {
                        console.log('icon button clicked')
                    }
                }

                EButton {
                    text: 'Icon Button 2'
                    icon: 'anchor'
                    onClicked: {
                        console.log('icon button clicked')
                    }
                }

                EButton {
                    icon: 'alarm'
                    onClicked: {
                        console.log('icon button clicked')
                    }
                }

                EButton {
                    icon: 'api'
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
                Layout.topMargin: 35

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
                Layout.topMargin: 35

                EText {
                    text: 'Icons'
                    style: EText.Style.Title1
                }
            }

            RowLayout {
                Layout.leftMargin: 5

                EIcon { name: 'arrow-left' }
                EIcon { name: 'arrow-left-up' }
                EIcon { name: 'arrow-up' }
                EIcon { name: 'arrow-right-up' }
                EIcon { name: 'arrow-right' }
                EIcon { name: 'arrow-right-down' }
                EIcon { name: 'arrow-down' }
                EIcon { name: 'arrow-left-down' }

                EIcon { name: 'arrow-left-circle' }
                EIcon { name: 'arrow-left-up-circle' }
                EIcon { name: 'arrow-up-circle' }
                EIcon { name: 'arrow-right-up-circle' }
                EIcon { name: 'arrow-right-circle' }
                EIcon { name: 'arrow-right-down-circle' }
                EIcon { name: 'arrow-down-circle' }
                EIcon { name: 'arrow-left-down-circle' }

                EIcon { name: 'caret-left' }
                EIcon { name: 'caret-up' }
                EIcon { name: 'caret-right' }
                EIcon { name: 'caret-down' }

                EIcon { name: 'chevron-left' }
                EIcon { name: 'chevron-up' }
                EIcon { name: 'chevron-right' }
                EIcon { name: 'chevron-down' }

                EIcon { name: 'chevron-left-double' }
                EIcon { name: 'chevron-up-double' }
                EIcon { name: 'chevron-right-double' }
                EIcon { name: 'chevron-down-double' }
            }

            RowLayout {
                Layout.leftMargin: 5

                EIcon { name: 'download' }
                EIcon { name: 'login' }
                EIcon { name: 'logout' }
                EIcon { name: 'fullscreen-1' }
                EIcon { name: 'fullscreen-exit-1' }
                EIcon { name: 'load' }
                EIcon { name: 'filter' }
                EIcon { name: 'dashboard' }
                EIcon { name: 'add-rectangle' }
                EIcon { name: 'app' }
                EIcon { name: 'hard-disk-storage' }
                EIcon { name: 'share' }
                EIcon { name: 'system-log' }
                EIcon { name: 'system-code' }
                EIcon { name: 'system-setting' }
                EIcon { name: 'hourglass' }
                EIcon { name: 'more' }
                EIcon { name: 'service' }
                EIcon { name: 'drag-move' }
                EIcon { name: 'focus' }
                EIcon { name: 'fill-color' }
                EIcon { name: 'sip' }
                EIcon { name: 'slice' }
                EIcon { name: 'copy' }
                EIcon { name: 'file-add' }
                EIcon { name: 'folder-add' }
                EIcon { name: 'folder' }
                EIcon { name: 'folder-import' }
            }

            RowLayout {
                Layout.leftMargin: 5
                Layout.topMargin: 35
                EText {
                    text: 'Switches'
                    style: EText.Style.Title1
                }
            }

            RowLayout {
                ESwitch {
                    text: 'Small Switch'
                    size: ESwitch.Size.Small
                }
                ESwitch {
                    text: 'Middle Switch'
                    size: ESwitch.Size.Middle
                }
                ESwitch {
                    text: 'Large Switch'
                    size: ESwitch.Size.Large
                }
            }

            RowLayout {
                ESwitch {
                    text: 'Disabled Checked'
                    disabled: true
                    checked: true
                }
                ESwitch {
                    text: 'Disabled Unchecked'
                    disabled: true
                    checked: false
                }
                ESwitch {
                    text: 'Filled Symbol'
                    filler: ESwitch.Filler.Text
                }
            }

            RowLayout {
                Layout.leftMargin: 5
                Layout.topMargin: 35
                EText {
                    text: 'TextInput'
                    style: EText.Style.Title1
                }
            }

            ColumnLayout {
                Layout.margins: 5

                RowLayout {
                    ETextField {
                        Layout.preferredWidth: 300
                    }

                    EText {
                        Layout.leftMargin: 5
                        text: 'Default'
                    }
                }

                RowLayout {
                    ETextField {
                        Layout.preferredWidth: 300
                        placeHolderText: 'Hello World'
                    }

                    EText {
                        Layout.leftMargin: 5
                        text: 'With Placeholder'
                    }
                }

                RowLayout {
                    ETextField {
                        Layout.preferredWidth: 300
                        wrapMode: TextInput.Wrap
                        text: 'This is a very very very very very very very very very long words.'
                    }

                    EText {
                        Layout.leftMargin: 5
                        text: 'Wrap'
                    }
                }

                RowLayout {
                    ETextField {
                        Layout.fillWidth: true
                        placeHolderText: 'Block Input'
                    }
                }
            }
        }
    }
}
