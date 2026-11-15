//
// Created by johnk on 2025/3/24.
//

#include <Common/Time.h>
#include <Launch/GameApplication.h>
#include <Core/Cmdline.h>
#include <RHI/Instance.h>
#include <Runtime/Engine.h>
#include <Runtime/SystemGraphPresets.h>
#include <Runtime/Settings/Registry.h>
#include <Runtime/Settings/Game.h>

namespace Launch {
    extern std::string gameModuleName;

    static Core::CmdlineArgValue<std::string> caRhiType(
        "rhiType", "-rhi", RHI::GetPlatformDefaultRHIAbbrString(),
        "rhi abbr string, can be 'dx12' or 'vulkan'");

    static Core::CmdlineArgValue<bool> caSoftwareGpu(
        "softwareGpu", "-softwareGpu", false,
        "prefer a software GPU, falling back to hardware when unavailable");

    GameApplication::GameApplication(int argc, char* argv[])
        : lastFrameTimeSeconds(Common::TimePoint::Now().ToSeconds())
        , thisFrameTimeSeconds(Common::TimePoint::Now().ToSeconds())
        , deltaTimeSeconds(0)
    {
        Core::Cli::Get().Parse(argc, argv);

        Runtime::EngineInitParams engineInitParams;
        engineInitParams.logToFile = true;
        engineInitParams.gpuDebug = false;
        engineInitParams.rhiType = caRhiType.GetValue();
        engineInitParams.useSoftwareGpu = caSoftwareGpu.GetValue();
        Runtime::EngineHolder::Load(gameModuleName, engineInitParams);

        engine = &Runtime::EngineHolder::Get();
        gameModule = &Core::ModuleManager::Get().GetTyped<Runtime::GameModule>(gameModuleName);

        GameWindowDesc windowDesc;
        windowDesc.title = gameModule->GetGameName();
        windowDesc.width = 1024;
        windowDesc.height = 768;
        auto& renderModule = engine->GetRenderModule();
        world = Common::MakeUnique<Runtime::World>("GameWorld", Runtime::PlayType::game);
        world->SetSystemGraph(Runtime::SystemGraphPresets::Default3DWorld());
        client = Common::MakeUnique<GameClient>(*world);
        window.Reset(new GameWindow(*renderModule.GetDevice(), renderModule.GetRenderThread(), *client, windowDesc));

        auto& settingRegistry = Runtime::SettingsRegistry::Get();
        settingRegistry.LoadAllSettings();

        const auto& gameSettings = Runtime::SettingsRegistry::Get().GetSettings<Runtime::GameSettings>();
        const auto startupLevel = Runtime::AssetManager::Get().SyncLoad<Runtime::Level>(gameSettings.gameStartupLevel, Runtime::Level::GetStaticClass());
        world->LoadFrom(startupLevel);
        world->Play();
    }

    GameApplication::~GameApplication()
    {
        window.Reset();
        if (!world->Stopped()) {
            world->Stop();
        }
        client.Reset();
        world.Reset();
        Runtime::EngineHolder::Unload();
    }

    void GameApplication::Tick()
    {
        thisFrameTimeSeconds = Common::TimePoint::Now().ToSeconds();
        deltaTimeSeconds = thisFrameTimeSeconds - lastFrameTimeSeconds;
        lastFrameTimeSeconds = thisFrameTimeSeconds;

        engine->Tick(deltaTimeSeconds);
    }

    bool GameApplication::ShouldClose() const
    {
        return window->ShouldClose();
    }
}
