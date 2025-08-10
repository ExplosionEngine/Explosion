//
// Created by johnk on 2025/8/8.
//

#include <Editor/WebUIServer.h>
#include <Core/Cmdline.h>

static Core::CmdlineArgValue<uint32_t> caWebUIPort(
    "webUIPort", "-webUIPort", 10907,
    "WebUI port");

namespace Editor {
    WebUIServer& WebUIServer::Get()
    {
        static WebUIServer webUIServer;
        return webUIServer;
    }

    void WebUIServer::Start()
    {
        serverThread = std::make_unique<Common::NamedThread>("WebUIServerThread", [this]() -> void {
            server = Common::MakeUnique<httplib::Server>();
            server->set_mount_point("/", "./Web");
            server->listen("localhost", caWebUIPort.GetValue());
        });
    }

    void WebUIServer::Stop()
    {
        server->stop();
        serverThread->Join();
        server.Reset();
        serverThread.Reset();
    }
} // namespace Editor
