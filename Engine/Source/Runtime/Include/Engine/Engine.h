//
// Created by johnk on 2022/7/18.
//

#pragma once

#include <string>

namespace Runtime {
    struct EngineInitializer {
        static EngineInitializer FromCommandline(int argc, char* argv);

        std::string engineRoot;
        std::string gameRoot;
    };

    class Engine {
    public:
        static Engine& Get();

        ~Engine();

        void Initialize(const EngineInitializer& initializer);
        void MainLoop();
        [[nodiscard]] const std::string& EngineRoot() const;
        [[nodiscard]] const std::string& GameRoot() const;

    protected:
        Engine();

        std::string engineRoot;
        std::string gameRoot;
    };
}
