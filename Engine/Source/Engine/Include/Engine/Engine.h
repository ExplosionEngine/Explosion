//
// Created by johnk on 2022/8/3.
//

#pragma once

#include <memory>

#include <Common/Path.h>
#include <Engine/Input.h>
#include <Engine/Config.h>

namespace Engine {
    struct EngineInitializer {
        std::string execFile;
        std::string projectFile;
        std::string map;
    };

    class Engine {
    public:
        static Engine& Get();
        ~Engine();

        void Initialize(const EngineInitializer& inInitializer);
        void Tick();
        Common::PathMapper& GetPathMapper();
        InputManager& GetInputManager();
        ConfigManager& GetConfigManager();

    private:
        Engine();

        void InitPathMapper(const std::string& execFile, const std::string& projectFile);
        void InitInputManager();
        void InitConfigManager();

        std::unique_ptr<Common::PathMapper> pathMapper;
        std::unique_ptr<InputManager> inputManager;
        std::unique_ptr<ConfigManager> configManager;
    };
}
