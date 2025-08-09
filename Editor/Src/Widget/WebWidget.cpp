//
// Created by johnk on 2025/8/9.
//

#include <Editor/Widget/WebWidget.h>
#include <Core/Cmdline.h>
#include <Editor/Widget/moc_WebWidget.cpp>

static Core::CmdlineArgValue<bool> caWebUIDev(
    "wevUIDev", "-wevUIDev", false,
    "Whether to enable hot reload for web UI");

static Core::CmdlineArgValue<uint32_t> caWebUIDevServerPort(
    "webUIDevServerPort", "-webUIDevServerPort", 5173,
    "Port of web ui dev server, which works only when dev mode enabled");

namespace Editor {
    WebWidget::WebWidget(QWidget* inParent)
        : QWebEngineView(inParent)
    {
    }

    WebWidget::~WebWidget() = default;

    void WebWidget::Load(const std::string& inUrl)
    {
        static Core::CmdlineArg& caWebUIPort = Core::Cli::Get().GetArg("webUIPort");

        Assert(inUrl.starts_with("/"));
        const std::string baseUrl = std::format("http://localhost:{}", caWebUIDev.GetValue() ? caWebUIDevServerPort.GetValue() : caWebUIPort.GetValue<uint32_t>());
        const std::string fullUrl = baseUrl + inUrl;
        load(QUrl(fullUrl.c_str()));
    }
} // namespace Editor
