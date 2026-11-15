//
// Created by johnk on 2024/10/31.
//

#include <Runtime/World.h>
#include <Runtime/Engine.h>

#include <utility>

namespace Runtime {
    World::World(std::string inName, PlayType inPlayType)
        : name(std::move(inName))
        , playStatus(PlayStatus::stopped)
        , systemSetupContext()
    {
        EngineHolder::Get().MountWorld(this);

        systemSetupContext.playType = inPlayType;
    }

    World::~World()
    {
        EngineHolder::Get().UnmountWorld(this);
    }

    void World::SetSystemGraph(const SystemGraph& inSystemGraph)
    {
        systemGraph = inSystemGraph;
    }

    Runtime::PlayStatus World::PlayStatus() const
    {
        return playStatus;
    }

    bool World::Stopped() const
    {
        return playStatus == PlayStatus::stopped;
    }

    bool World::Playing() const
    {
        return playStatus == PlayStatus::playing;
    }

    bool World::Paused() const
    {
        return playStatus == PlayStatus::paused;
    }

    void World::Play()
    {
        Assert(Stopped() && !executor.has_value());
        playStatus = PlayStatus::playing;
        executor.emplace(ecRegistry, systemGraph, systemSetupContext);
    }

    void World::Resume()
    {
        Assert(Paused());
        playStatus = PlayStatus::playing;
    }

    void World::Pause()
    {
        Assert(Playing());
        playStatus = PlayStatus::paused;
    }

    void World::Stop()
    {
        Assert((Playing() || Paused()) && executor.has_value());
        playStatus = PlayStatus::stopped;
        executor.reset();
    }

    bool World::ShouldTick() const
    {
        return executor.has_value() && !Paused();
    }

    void World::LoadFrom(AssetPtr<Level> inLevel)
    {
        Assert(Stopped());
        ecRegistry.Load(inLevel->GetArchive());
    }

    void World::SaveTo(AssetPtr<Level> inLevel)
    {
        // an editor world keeps playing while editing, saving there is safe because transient entities are skipped
        Assert(systemSetupContext.playType == PlayType::editor || Stopped());
        ecRegistry.Save(inLevel->GetArchive());
    }

#if BUILD_EDITOR
    void World::EditorAccess(const std::function<void(ECRegistry&)>& inAccessFunc)
    {
        Assert(systemSetupContext.playType == PlayType::editor);
        inAccessFunc(ecRegistry);
    }
#endif

    void World::Tick(float inDeltaTimeSeconds)
    {
        executor->Tick(inDeltaTimeSeconds);
    }
} // namespace Runtime
