import QtQuick
import QtQuick.Controls

Item {
    enum Style {
        Title1,
        Title2,
        Title3,
        Content,
        Link,
        Italic
    }

    property string text: ''
    property string href: ''
    property int style: EText.Style.Content

    id: 'root'
    implicitWidth: textWidget.implicitWidth
    implicitHeight: textWidget.implicitHeight

    Text {
        function getText(text, href, style)
        {
            return style === EText.Style.Link
                ? '<a href=\"%1\" style=\"color: %2; text-decoration: none;\">%3</a>'.arg(href).arg(ETheme.linkFontColor).arg(text)
                : text;
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

        id: 'textWidget'
        text: getText(root.text, root.href, root.style)
        textFormat: root.style === EText.Style.Link ? Text.RichText : Text.PlainText;
        font.italic: root.style === EText.Style.Italic
        font.bold: root.style < EText.Style.Content;
        font.pixelSize: getFontPixelSize(root.style)
        font.family: ETheme.fontFamily
        color: ETheme.fontColor
        onLinkActivated: Qt.openUrlExternally(root.href)
    }
}
