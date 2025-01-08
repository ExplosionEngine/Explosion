pragma Singleton

import QtQuick

QtObject {
    property color bgColor: Qt.color('#212121')
    property color primaryColor: Qt.color('#e74c3c')
    property color focusColor: Qt.color('#c0392b')
    property color secondaryColor: Qt.color('#f1c40f')
    property color fontColor: Qt.color('#ecf0f1')

    property FontLoader fontNormal: FontLoader { source: Qt.url('Font/MiSans-Normal.ttf') }
    property FontLoader fontBold: FontLoader { source: Qt.url('Font/MiSans-Bold.ttf') }
    property int fontSizeMain: 14
    property string fontFamily: 'MiSans'
}
