//
// Created by johnk on 2024/8/21.
//

#include <Common/Debug.h>
#include <Common/Time.h>
#include <Core/Console.h>
#include <Core/Log.h>
#include <Core/Module.h>
#include <Core/Paths.h>
#include <Core/Thread.h>
#include <Mirror/Mirror.h>
#include <Runtime/Engine.h>
#include <Runtime/GameThread.h>
#include <Runtime/Settings/Registry.h>
#include <Runtime/World.h>

namespace Runtime {
    Engine::Engine(const EngineInitParams& inParams)
    {
        Core::ThreadContext::SetTag(Core::ThreadTag::game);
        GameWorkerThreads::Get().Start();

        if (!inParams.gameRoot.empty()) {
            Core::Paths::SetGameRoot(inParams.gameRoot);
        }

        if (inParams.logToFile) {
            AttachLogFile();
        }
        InitRender(inParams.rhiType);
        LoadPlugins();
        LoadConfigs();
    }

    Engine::~Engine()
    {
        renderModule->DeInitialize();
        ::Core::ModuleManager::Get().Unload("Render");

        GameWorkerThreads::Get().Stop();
    }

    void Engine::MountWorld(World* inWorld)
    {
        worlds.emplace(inWorld);
    }

    void Engine::UnmountWorld(World* inWorld)
    {
        worlds.erase(inWorld);
    }

    Render::RenderModule& Engine::GetRenderModule() const
    {
        return *renderModule;
    }

    void Engine::Tick(float inDeltaTimeSeconds)
    {
        // game thread can run faster than render thread 1 frame as max
        if (last2FrameRenderThreadFence.valid()) {
            last2FrameRenderThreadFence.wait();
        }

        Core::ThreadContext::IncFrameNumber();

        auto& renderThread = renderModule->GetRenderThread();
        renderThread.EmplaceTask([]() -> void {
            Core::ThreadContext::IncFrameNumber();
            Core::Console::Get().PerformRenderThreadSettingsCopy();
        });

        for (auto* world : worlds) {
            if (!world->Playing()) {
                continue;
            }
            world->Tick(inDeltaTimeSeconds);
        }

        GameThread::Get().Flush();
        last2FrameRenderThreadFence = std::move(lastFrameRenderThreadFence);
        lastFrameRenderThreadFence = renderThread.EmplaceTask([]() -> void {});
    }

    void Engine::AttachLogFile() const // NOLINT
    {
        const auto time = Common::Time(Common::TimePoint::Now());
        const auto logName = Core::Paths::ExecutablePath().FileNameWithoutExtension() + "-" + time.ToString() + ".log";
        const auto logFile = ((Core::Paths::HasSetGameRoot() ? Core::Paths::GameLogDir() : Core::Paths::EngineLogDir()) / logName).String();

        Core::Logger::Get().Attach(new Core::FileLogStream(logFile));
        LogInfo(Core, "logger attached to file {}", logFile);
    }

    void Engine::InitRender(const std::string& inRhiTypeStr)
    {
        renderModule = ::Core::ModuleManager::Get().FindOrLoadTyped<Render::RenderModule>("Render");
        Assert(renderModule != nullptr);

        Render::RenderModuleInitParams initParams;
        initParams.rhiType = RHI::GetRHITypeByAbbrString(inRhiTypeStr);
        renderModule->Initialize(initParams);
        LogInfo(Render, "RHI type: {}", inRhiTypeStr);
    }

    void Engine::LoadPlugins() const // NOLINT
    {
        // TODO
    }

    void Engine::LoadConfigs() const // NOLINT
    {
        Core::Console::Get().OverrideSettingsByConfig();
        SettingsRegistry::Get().LoadAllSettings();
    }

    Common::UniquePtr<Engine> EngineHolder::engine = nullptr;

    MinEngine::MinEngine(const EngineInitParams& inParams)
        : Engine(inParams)
    {
    }

    MinEngine::~MinEngine() = default;

    bool MinEngine::IsEditor()
    {
        return false;
    }

    void EngineHolder::Load(const std::string& inModuleName, const EngineInitParams& inInitParams)
    {
        Assert(engine == nullptr);
        auto& gameModule = Core::ModuleManager::Get().GetOrLoadTyped<IEngineModule>(inModuleName);
        engine = gameModule.CreateEngine(inInitParams);
    }

    void EngineHolder::Unload()
    {
        Assert(engine != nullptr);
        engine = nullptr;
    }

    Engine& EngineHolder::Get()
    {
        AssertWithReason(engine != nullptr, "no valid engine");
        return *engine;
    }
}
