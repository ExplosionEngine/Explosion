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

    System::System() = default;

    System::~System() = default;

    void System::Setup(Commands& inCommands) const {}

    void System::Tick(Commands& inCommands, float inTimeMs) const {}

    void System::Stop(Commands& inCommands) const {}

    void Observer::Clear()
    {
        observer.clear();
    }

    Observer::Iterator Observer::Begin() const
    {
        return observer.begin();
    }

    Observer::Iterator Observer::End() const
    {
        return observer.end();
    }

    Observer::Iterator Observer::begin() const
    {
        return Begin();
    }

    Observer::Iterator Observer::end() const
    {
        return End();
    }

    Commands::Commands(EntityCompRegistry& inRegistry)
        : registry(inRegistry)
    {
    }

    Commands::~Commands() = default;

    Entity Commands::CreateEntity() // NOLINT
    {
        return registry.native.create();
    }

    void Commands::DestroyEntity(Entity inEntity) // NOLINT
    {
        registry.native.destroy(inEntity);
    }

    bool Commands::HasEntity(Entity inEntity) const
    {
        return registry.native.valid(inEntity);
    }

    World::World(std::string inName)
        : setup(false)
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
        systemRegistry.systemsGraph.emplace_back(systemRegistry.systemsInBarriers);
        systemRegistry.systemsInBarriers.clear();
    }

    void World::Play()
    {
        Assert(!setup && systemRegistry.systemsInBarriers.empty());
        setup = true;
        playing = true;
        ExecuteSystemGraph([&](const System& inSystem) -> void {
            Commands commands(entityCompRegistry);
            inSystem.Setup(commands);
        });
    }

    void World::Stop()
    {
        Assert(setup);
        setup = false;
        playing = false;
    }

    void World::Pause()
    {
        Assert(setup && playing);
        playing = false;
    }

    void World::Resume()
    {
        Assert(setup && !playing);
        playing = true;
    }

    void World::Tick(float inFrameTimeMs)
    {
        Assert(setup && playing);
        ExecuteSystemGraph([&](const System& inSystem) -> void {
            Commands commands(entityCompRegistry);
            inSystem.Tick(commands, inFrameTimeMs);
        });
    }

    bool World::Started() const
    {
        return setup;
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
        for (const auto& systemSet : systemRegistry.systemsGraph) {
            std::vector<tf::Task> tasks;
            for (const auto& systemClass : systemSet) {
                auto& addedTask = tasks.emplace_back(taskFlow.emplace([&]() -> void {
                    inOp(systemRegistry.systems.at(systemClass).As<const System&>());
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
