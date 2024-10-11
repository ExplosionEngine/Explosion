//
// Created by johnk on 2024/8/2.
//

#include <Runtime/World.h>
#include <Runtime/Engine.h>

namespace Runtime {
    const Mirror::Class* Internal::GetClassChecked(const std::string& inName)
    {
        return &Mirror::Class::Get(inName);
    }

    Component::Component() = default;

    State::State() = default;

    System::System() = default;

    System::~System() = default;

    void System::Setup(Commands& commands) const {}

    void System::Tick(Commands& commands, float inTimeMs) const {}

    Commands::Commands(World& inWorld)
        : world(inWorld)
    {
    }

    Commands::~Commands() = default;

    Entity Commands::CreateEntity() // NOLINT
    {
        return world.registry.create();
    }

    void Commands::DestroyEntity(Entity inEntity) // NOLINT
    {
        world.registry.destroy(inEntity);
    }

    World::World(std::string inName)
        : setuped(false)
        , playing(false)
        , name(std::move(inName))
    {
        EngineHolder::Get().MountWorld(this);
    }

    World::~World()
    {
        EngineHolder::Get().UnmountWorld(this);
    }

    void World::AddBarrier()
    {
        systemsGraph.emplace_back(systemsInBarriers);
        systemsInBarriers.clear();
    }

    void World::Play()
    {
        Assert(!setuped && systemsInBarriers.empty());
        setuped = true;
        playing = true;
        ExecuteSystemGraph([&](const System& inSystem) -> void {
            Commands commands(*this);
            inSystem.Setup(commands);
        });
    }

    void World::Stop()
    {
        Assert(setuped);
        setuped = false;
        playing = false;
    }

    void World::Pause()
    {
        Assert(setuped && playing);
        playing = false;
    }

    void World::Resume()
    {
        Assert(setuped && !playing);
        playing = true;
    }

    void World::Tick(float inFrameTimeMs)
    {
        Assert(setuped && playing);
        ExecuteSystemGraph([&](const System& inSystem) -> void {
            Commands commands(*this);
            inSystem.Tick(commands, inFrameTimeMs);
        });
    }

    bool World::Started() const
    {
        return setuped;
    }

    bool World::Playing() const
    {
        return playing;
    }

    void World::ExecuteSystemGraph(const SystemOp& inOp)
    {
        tf::Taskflow taskFlow;
        auto newBarrierTask = [&]() -> decltype(auto) {
            return taskFlow.emplace([]() -> void {});
        };

        tf::Task barrierTask = newBarrierTask();
        for (const auto& systemSet : systemsGraph) {
            std::vector<tf::Task> tasks;
            for (const auto& systemClass : systemSet) {
                auto& addedTask = tasks.emplace_back(taskFlow.emplace([&]() -> void {
                    inOp(systems.at(systemClass).As<const System&>());
                }));
                addedTask.succeed(barrierTask);
            }

            barrierTask = newBarrierTask();
            for (const auto& task : tasks) {
                barrierTask.succeed(task);
            }
        }

        tf::Executor executor;
        executor
            .run(taskFlow)
            .wait();
    }
}
