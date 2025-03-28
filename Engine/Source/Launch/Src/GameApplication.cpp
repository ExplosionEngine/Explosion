//
// Created by johnk on 2025/3/24.
//

#include <Common/Time.h>
#include <Launch/GameApplication.h>
#include <Core/Cmdline.h>
#include <RHI/Instance.h>
#include <Runtime/Engine.h>
#include <Runtime/Settings/Registry.h>
#include <Runtime/Settings/Game.h>

namespace Launch {
    extern std::string gameModuleName;

    static Core::CmdlineArgValue<std::string> caRhiType(
        "rhiType", "-rhi", RHI::GetPlatformDefaultRHIAbbrString(),
        "rhi abbr string, can be 'dx12' or 'vulkan'");

    GameApplication::GameApplication(int argc, char* argv[])
        : lastFrameTimeSeconds(Common::TimePoint::Now().ToSeconds())
        , thisFrameTimeSeconds(Common::TimePoint::Now().ToSeconds())
        , deltaTimeSeconds(0)
    {
        Core::Cli::Get().Parse(argc, argv);

        Runtime::EngineInitParams engineInitParams;
        engineInitParams.logToFile = true;
        engineInitParams.rhiType = caRhiType.GetValue();
        Runtime::EngineHolder::Load(gameModuleName, engineInitParams);

        engine = &Runtime::EngineHolder::Get();
        gameModule = &Core::ModuleManager::Get().GetTyped<Runtime::GameModule>(gameModuleName);

        GameViewportDesc viewportDesc;
        viewportDesc.title = gameModule->GetGameName();
        // TODO load from config
        viewportDesc.width = 1024;
        viewportDesc.height = 768;
        viewport = Common::MakeUnique<GameViewport>(viewportDesc);

        auto& settingRegistry = Runtime::SettingsRegistry::Get();
        settingRegistry.LoadAllSettings();

        const auto& gameSettings = Runtime::SettingsRegistry::Get().GetSettings<Runtime::GameSettings>();
        const auto startupLevel = Runtime::AssetManager::Get().SyncLoad<Runtime::Level>(gameSettings.gameStartupLevel, Runtime::Level::GetStaticClass());
        viewport->GetClient().GetWorld().LoadFrom(startupLevel);
    }

    GameApplication::~GameApplication()
    {
        Runtime::EngineHolder::Unload();
    }

    void GameApplication::Tick()
    {
        thisFrameTimeSeconds = Common::TimePoint::Now().ToSeconds();
        deltaTimeSeconds = thisFrameTimeSeconds - lastFrameTimeSeconds;
        lastFrameTimeSeconds = thisFrameTimeSeconds;

        engine->Tick(deltaTimeSeconds);
        viewport->PollEvents();
    }

    bool GameApplication::ShouldClose() const
    {
        return viewport->ShouldClose();
    }
}
