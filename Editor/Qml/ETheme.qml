pragma Singleton

import QtQuick

QtObject {
    property color bgColor: Qt.color('#212121')
    property color primaryBgColor: Qt.color('#2a2a2a')
    property color primaryColor: Qt.color('#e74c3c')
    property color primaryHoverColor: Qt.color('#ce4d40')
    property color primaryFocusColor: Qt.color('#c0392b')
    property color secondaryColor: Qt.color('#d58845')
    property color secondaryHoverColor: Qt.color('#d58845')
    property color secondaryFocusColor: Qt.color('#9b6a40')
    property color disabledColor: Qt.color('#676563')
    property color fontColor: Qt.color('#ecf0f1')
    property color linkFontColor: Qt.color('#91b9c4')

    property FontLoader normalFont: FontLoader { source: Qt.url('Resource/Font/MiSans-Normal.ttf') }
    property FontLoader boldFont: FontLoader { source: Qt.url('Resource/Font/MiSans-Bold.ttf') }
    property int tiele1FontSize: 20
    property int title2FontSize: 18
    property int title3FontSize: 16
    property int contentFontSize: 14
    property int iconFontSize: 18
    property string fontFamily: 'MiSans'
}
