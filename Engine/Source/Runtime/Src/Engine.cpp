//
// Created by johnk on 2022/8/3.
//

#include <Runtime/Engine.h>
#include <Runtime/Application.h>
#include <Runtime/World.h>
#include <Common/Debug.h>
#include <Common/Memory.h>

namespace Runtime{
    Engine& Engine::Get()
    {
        static Engine instance;
        return instance;
    }

    Engine::Engine() : application(nullptr), activeWorld(nullptr) {}

    Engine::~Engine() = default;

    void Engine::Initialize(const EngineInitializer& inInitializer)
    {
        application = inInitializer.application;
        InitPathMapper(inInitializer.execFile, inInitializer.projectFile);
        InitInputManager();
        InitConfigManager();
        InitAssetManager();
        for (const auto& listener : listeners.onInits) {
            listener();
        }
    }

    void Engine::Tick()
    {
        // TODO

        for (const auto& listener : listeners.onTicks) {
            listener();
        }
    }

    IApplication* Engine::GetApplication() const
    {
        return application;
    }

    Common::PathMapper& Engine::GetPathMapper() const
    {
        return *pathMapper;
    }

    InputManager& Engine::GetInputManager() const
    {
        return *inputManager;
    }

    ConfigManager& Engine::GetConfigManager() const
    {
        return *configManager;
    }

    AssetManager& Engine::GetAssetManager() const
    {
        return *assetManager;
    }

    void Engine::SetActiveWorld(World* inWorld)
    {
        inWorld->Setup();
        activeWorld = inWorld;
    }

    void Engine::AddOnInitListener(std::function<void()> listener)
    {
        listeners.onInits.emplace_back(std::move(listener));
    }

    void Engine::AddOnTickListener(std::function<void()> listener)
    {
        listeners.onTicks.emplace_back(std::move(listener));
    }

    void Engine::InitPathMapper(const std::string& execFile, const std::string& projectFile)
    {
        std::unordered_map<std::string, std::string> pathMap;
        pathMap["/Engine"] = Common::PathUtils::GetParentPath(execFile);
        pathMap["/Game"] = Common::PathUtils::GetParentPath(projectFile);
        pathMapper = std::make_unique<Common::PathMapper>(Common::PathMapper::From(pathMap));
    }

    void Engine::InitInputManager()
    {
        inputManager = Common::UniqueRef<InputManager>(new InputManager);
    }

    void Engine::InitConfigManager()
    {
        configManager = Common::UniqueRef<ConfigManager>(new ConfigManager(*pathMapper));
    }

    void Engine::InitAssetManager()
    {
        assetManager = Common::UniqueRef<AssetManager>(new AssetManager(*pathMapper));
    }
}
