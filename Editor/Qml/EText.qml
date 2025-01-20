import QtQuick
import QtQuick.Controls

Text {
    enum Style {
        Title1,
        Title2,
        Title3,
        Content
    }

    function getFontBold(style)
    {
        return style !== EText.Style.Content
    }

    function getFontPixelSize(style)
    {
        if (style === EText.Style.Title1) {
            return ETheme.tiele1FontSize;
        } else if (style === EText.Style.Title2) {
            return ETheme.title2FontSize;
        } else if (style === EText.Style.Title3) {
            return ETheme.title3FontSize;
        } else {
            return ETheme.contentFontSize;
        }
    }

    property int style: EText.Style.Content

    font.bold: getFontBold(style)
    font.pixelSize: getFontPixelSize(style)
    font.family: ETheme.fontFamily
    color: ETheme.fontColor
}
