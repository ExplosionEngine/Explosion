//
// Created by johnk on 2023/9/5.
//

#include <taskflow/taskflow.hpp>

#include <Runtime/World.h>

namespace Runtime {
    SystemSchedule::SystemSchedule(World& inWorld, SystemSignature inTarget)
        : world(inWorld)
        , target(inTarget)
    {
    }

    SystemSchedule::~SystemSchedule() = default;

    SystemSchedule& SystemSchedule::ScheduleAfterInternal(SystemSignature depend)
    {
        Assert(world.systemDependencies.contains(target));
        world.systemDependencies.at(target).emplace_back(depend);
        return *this;
    }

    EventSlot::EventSlot(World& inWorld, SystemEventSignature inTarget)
        : world(inWorld)
        , target(inTarget)
    {
    }

    EventSlot::~EventSlot() = default;

    World::World(std::string inName)
        : ECSHost()
        , name(std::move(inName))
        , setuped(false)
    {
    }

    World::~World() = default;

    void World::BroadcastSystemEvent(SystemEventSignature eventSignature, const Mirror::Any& eventRef)
    {
        if (!systemEventSlots.contains(eventSignature)) {
            return;
        }

        SystemCommands commands(registry, *this);
        for (const auto& systemId : systemEventSlots.at(eventSignature)) {
            Assert(systems.contains(systemId));
            auto* system = systems.at(systemId).Get();
            static_cast<EventSystem*>(system)->OnReceiveEvent(commands, eventRef);
        }
    }

    void World::Setup()
    {
        Assert(!setuped);
        ExecuteWorkSystems(setupSystems);
        setuped = true;
    }

    void World::Shutdown()
    {
        Assert(setuped);
        setuped = false;
    }

    void World::Tick()
    {
        Assert(setuped);
        ExecuteWorkSystems(tickSystems);
    }

    SystemSignature World::CreateSystem(SystemSignature systemId, System* systemInstance)
    {
        Assert(!systems.contains(systemId) && !systemDependencies.contains(systemId));
        systems.emplace(std::make_pair(systemId, Common::UniqueRef<System>(systemInstance)));
        systemDependencies.emplace(std::make_pair(systemId, std::vector<SystemSignature> {}));
        return systemId;
    }

    void World::ExecuteWorkSystems(const std::vector<SystemSignature>& targets)
    {
        tf::Taskflow taskflow;
        std::unordered_map<SystemSignature, tf::Task> tasks;

        tasks.clear();
        tasks.reserve(targets.size());

        SystemCommands commands(registry, *this);
        for (SystemSignature target : targets) {
            tasks.emplace(std::make_pair(target, taskflow.emplace([this, &commands, target]() -> void {
                Assert(systems.contains(target));
                auto* system = systems.at(target).Get();
                static_cast<WorkSystem*>(system)->Execute(commands);
            })));
        }

        for (auto& pair : tasks) {
            Assert(systemDependencies.contains(pair.first));
            const auto& dependSystemIds = systemDependencies.at(pair.first);
            for (const auto& dependSystemId : dependSystemIds) {
                Assert(tasks.contains(dependSystemId));
                pair.second.succeed(tasks.at(dependSystemId));
            }
        }

        tf::Executor executor;
        executor.run(taskflow).wait();
    }

#if BUILD_TEST
    WorldTestHelper::WorldTestHelper(World& inWorld)
        : world(inWorld)
    {
    }

    WorldTestHelper::~WorldTestHelper() = default;

    SystemCommands WorldTestHelper::HackCreateSystemCommands()
    {
        return SystemCommands(world.registry, world);
    }
#endif
}
