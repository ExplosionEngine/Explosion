//
// Created by johnk on 2025/8/9.
//

#include <Editor/Widget/WebWidget.h>
#include <Editor/Widget/moc_WebWidget.cpp>
#include <Core/Cmdline.h>
#include <Core/Log.h>
#include <Editor/WebUIServer.h>

namespace Editor {
    WebPage::WebPage(QWidget* inParent)
        : QWebEnginePage(inParent)
    {
    }

    WebPage::~WebPage() = default;

    void WebPage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString& message, int lineNumber, const QString& sourceID)
    {
        if (level == InfoMessageLevel)
        {
            LogInfo(WebUIJavaScript, "{}", message.toStdString());
        }
        else if (level == WarningMessageLevel)
        {
            LogWarning(WebUIJavaScript, "{}", message.toStdString());
        }
        else if (level == ErrorMessageLevel)
        {
            LogError(WebUIJavaScript, "{}", message.toStdString());
        }
    }

    WebWidget::WebWidget(QWidget* inParent)
        : QWebEngineView(inParent)
    {
        webChannel = new QWebChannel(this);
        setPage(new WebPage(this));
        page()->setWebChannel(webChannel);
    }

    WebWidget::~WebWidget() = default;

    void WebWidget::Load(const std::string& inUrl)
    {
        static Core::CmdlineArg& caWebUIPort = Core::Cli::Get().GetArg("webUIPort");

        Assert(inUrl.starts_with("/"));
        const auto& baseUrl = WebUIServer::Get().BaseUrl();
        const auto fullUrl = baseUrl + inUrl;
        load(QUrl(fullUrl.c_str()));
    }

    QWebChannel* WebWidget::GetWebChannel() const
    {
        return webChannel;
    }
} // namespace Editor
