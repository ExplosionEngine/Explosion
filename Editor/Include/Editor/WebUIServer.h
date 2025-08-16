//
// Created by johnk on 2025/8/8.
//

#pragma once

#include <httplib.h>

#include <Common/Concurrent.h>
#include <Common/Memory.h>

namespace Editor {
    class WebUIServer {
    public:
        static WebUIServer& Get();

        void Start();
        void Stop();
        const std::string& BaseUrl() const;

    private:
        WebUIServer();

        std::string baseUrl;
        Common::UniquePtr<Common::NamedThread> productServerThread;
        Common::UniquePtr<httplib::Server> productServer;
    };
}
