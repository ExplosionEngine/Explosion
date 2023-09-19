//
// Created by johnk on 2023/9/5.
//

#include <taskflow/taskflow.hpp>

#include <Runtime/World.h>

namespace Runtime {
    SystemSchedule::SystemSchedule() = default;

    SystemSchedule::~SystemSchedule() = default;

    void SystemSchedule::InvokeSchedule(World& world, SystemTypeId target) const
    {
        for (const auto& func : scheduleFuncs) {
            func(world, target);
        }
    }

    EventSlot::EventSlot() = default;

    EventSlot::~EventSlot() = default;

    void EventSlot::InvokeConnect(World& world, SystemEventTypeId target) const
    {
        for (const auto& func : connectFuncs) {
            func(world, target);
        }
    }

    World::World(std::string inName)
        : name(std::move(inName))
    {
    }

    World::~World() = default;

    void World::BroadcastSystemEvent(Mirror::TypeId eventTypeId, const Mirror::Any& eventRef)
    {
        SystemCommands systemCommands(registry, *this);
        Assert(systemEventSlots.contains(eventTypeId));
        for (const auto& systemId : systemEventSlots.at(eventTypeId)) {
            Assert(systems.contains(systemId));
            auto* system = systems.at(systemId).Get();
            static_cast<EventSystem*>(system)->OnReceiveEvent(systemCommands, eventRef);
        }
    }

    void World::Setup()
    {
        Assert(!setuped);
        ExecuteSystems(setupSystems);
    }

    void World::Shutdown()
    {
        Assert(setuped);
        setuped = false;
    }

    void World::Tick()
    {
        Assert(setuped);
        ExecuteSystems(tickSystems);
    }

    void World::ExecuteSystems(const std::vector<SystemTypeId>& targets)
    {
        tf::Taskflow taskflow;
        std::unordered_map<SystemTypeId, tf::Task> tasks;

        tasks.clear();
        tasks.reserve(targets.size());

        SystemCommands systemCommands(registry, *this);
        for (SystemTypeId target : targets) {
            tasks.emplace(std::make_pair(target, taskflow.emplace([&]() -> void {
                Assert(systems.contains(target));
                auto* system = systems.at(target).Get();
                static_cast<WorkSystem*>(system)->Execute(systemCommands);
            })));
        }

        for (auto& pair : tasks) {
            Assert(systemDependencies.contains(pair.first));
            const auto& dependSystemIds = systemDependencies.at(pair.first);
            for (auto dependSystemId : dependSystemIds) {
                Assert(tasks.contains(dependSystemId));
                tasks.at(dependSystemId).succeed(pair.second);
            }
        }

        tf::Executor executor;
        executor.run(taskflow).wait();
    }
}
