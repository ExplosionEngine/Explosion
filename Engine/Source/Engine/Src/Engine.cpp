//
// Created by johnk on 2022/8/3.
//

#include <Engine/Engine.h>
#include <Common/Debug.h>

namespace Engine {
    Engine& Engine::Get()
    {
        static Engine instance;
        return instance;
    }

    Engine::Engine() = default;

    Engine::~Engine() = default;

    void Engine::Initialize(const EngineInitializer& inInitializer)
    {
        InitPathMapper(inInitializer.execFile, inInitializer.projectFile);
        InitInputManager();
        InitConfigManager();
    }

    void Engine::Tick()
    {
        // TODO
    }

    Common::PathMapper& Engine::GetPathMapper()
    {
        return *pathMapper;
    }

    InputManager& Engine::GetInputManager()
    {
        return *inputManager;
    }

    ConfigManager& Engine::GetConfigManager()
    {
        return *configManager;
    }

    void Engine::InitPathMapper(const std::string& execFile, const std::string& projectFile)
    {
        // TODO
    }

    void Engine::InitInputManager()
    {
        inputManager = std::make_unique<InputManager>();
    }

    void Engine::InitConfigManager()
    {
        configManager = std::make_unique<ConfigManager>(*pathMapper);
    }
}
