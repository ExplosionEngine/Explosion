pragma Singleton

import QtQuick

QtObject {
    property color bgColor: Qt.color('#212121')
    property color primaryColor: Qt.color('#e74c3c')
    property color focusColor: Qt.color('#c0392b')
    property color secondaryColor: Qt.color('#f1c40f')
    property color fontColor: Qt.color('#ecf0f1')

    property FontLoader normalFont: FontLoader { source: Qt.url('Resource/Font/MiSans-Normal.ttf') }
    property FontLoader boldFont: FontLoader { source: Qt.url('Resource/Font/MiSans-Bold.ttf') }
    property int tiele1FontSize: 20
    property int title2FontSize: 18
    property int title3FontSize: 16
    property int contentFontSize: 14
    property string fontFamily: 'MiSans'
}
