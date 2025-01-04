import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    color: ETheme.bgColor

    ColumnLayout {
        width: parent.width

        RowLayout {
            EButton {
                text: 'Hello'
            }
        }
    }
}
