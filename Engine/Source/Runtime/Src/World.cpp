//
// Created by johnk on 2024/10/31.
//

#include <Runtime/World.h>
#include <Runtime/Engine.h>

namespace Runtime {
    World::World(const std::string& inName)
        : name(inName)
        , playStatus(PlayStatus::stopped)
    {
        EngineHolder::Get().MountWorld(this);
    }

    World::~World()
    {
        EngineHolder::Get().UnmountWorld(this);
    }

    void World::SetSystemGraph(const SystemGraph& inSystemGraph)
    {
        systemGraph = inSystemGraph;
    }

    void World::Reset()
    {
        playStatus = PlayStatus::stopped;
    }

    PlayStatus World::PlayStatus() const
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
        executor.emplace(ecRegistry, systemGraph);
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

    void World::Tick(float inDeltaTimeSeconds)
    {
        executor->Tick(inDeltaTimeSeconds);
    }
} // namespace Runtime
