//
// Created by johnk on 2025/8/8.
//

#include <QtEnvironmentVariables>
#include <QByteArrayView>

#include <Core/Cmdline.h>
#include <Core/Log.h>
#include <Core/Paths.h>
#include <Editor/WebUIServer.h>

static Core::CmdlineArgValue<uint32_t> caWebUIPort(
    "webUIPort", "-webUIPort", 10907,
    "WebUI port");

static Core::CmdlineArgValue<bool> caWebUIDev(
    "webUIDev", "-webUIDev", false,
    "Whether to enable hot reload for web UI");

static Core::CmdlineArgValue<uint32_t> caWebUIDevServerPort(
    "webUIDevServerPort", "-webUIDevServerPort", 5173,
    "Port of web ui dev server, which works only when dev mode enabled");

static Core::CmdlineArgValue<bool> caWebUIDebug(
    "webUIDebug", "-webUIDebug", false,
    "Whether to enable web ui debug (you can attach debugger to qt web engine process).");

static Core::CmdlineArgValue<uint32_t> caWebUIRemoteDebugPort(
    "webUIRemoveDebugPort", "-webUIRemoveDebugPort", 5174,
    "Port of web ui debug port, you can attach to the url printed in log to create debug process.");

namespace Editor {
    WebUIServer& WebUIServer::Get()
    {
        static WebUIServer webUIServer;
        return webUIServer;
    }

    WebUIServer::WebUIServer() = default;

    void WebUIServer::Start()
    {
        uint32_t serverPort;
        std::string serverMode;

        if (caWebUIDev.GetValue()) {
            serverPort = caWebUIDevServerPort.GetValue();
            serverMode = "development";

            httplib::Client client(std::format("http://localhost:{}", serverPort));
            auto res = client.Get("/");
            AssertWithReason(
                res->status == 200,
                "did you forget to start dev server, just call Script/start_editor_web_dev_server.py manually");
        } else {
            serverPort = caWebUIPort.GetValue();
            serverMode = "product";
            productServerThread = std::make_unique<Common::NamedThread>("WebUIServerThread", [this, serverPort]() -> void {
                const auto webRoot = Core::Paths::ExecutablePath().Parent() / "Web";
                const auto indexHtmlFile = webRoot / "index.html";

                productServer = Common::MakeUnique<httplib::Server>();
                productServer->set_mount_point("/", webRoot.Absolute().String());
                productServer->Get("/(.+)", [indexHtmlFile](const httplib::Request&, httplib::Response& res) {
                    res.set_file_content(indexHtmlFile.Absolute().String());
                });
                productServer->listen("localhost", static_cast<int32_t>(serverPort));
            });
        }

        baseUrl = std::format("http://localhost:{}", serverPort);
        LogInfo(WebUI, "{} web ui server listening on {}", serverMode, baseUrl);

        if (caWebUIDebug.GetValue()) {
            const auto flags = std::format("--remote-debugging-port={}", caWebUIRemoteDebugPort.GetValue());
            qputenv("QTWEBENGINE_CHROMIUM_FLAGS", QByteArrayView(flags.c_str(), static_cast<qsizetype>(flags.length())));
        }
    }

    void WebUIServer::Stop()
    {
        if (productServer.Valid()) {
            productServer->stop();
        }
        if (productServerThread.Valid()) {
            productServerThread->Join();
        }
        productServer.Reset();
        productServerThread.Reset();
    }

    const std::string& WebUIServer::BaseUrl() const
    {
        return baseUrl;
    }
} // namespace Editor
