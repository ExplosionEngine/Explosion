//
// Created by johnk on 2022/7/18.
//

#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include <Common/Path.h>

namespace Runtime {
    struct EngineInitializer {
        static EngineInitializer FromCommandline(int argc, char* argv);

        std::unordered_map<std::string, std::string> pathMap;
    };

    class Engine {
    public:
        static Engine& Get();

        ~Engine();

        void Initialize(const EngineInitializer& initializer);
        void MainLoop();
        [[nodiscard]] const Common::PathMapper& GetPathMapper() const;

    protected:
        Engine();

    private:
        std::unique_ptr<Common::PathMapper> pathMapper;
    };
}
