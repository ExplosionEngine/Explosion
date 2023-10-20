//
// Created by johnk on 2023/9/5.
//

#include <taskflow/taskflow.hpp>

#include <Runtime/World.h>

namespace Runtime {
    SystemSchedule::SystemSchedule(World& inWorld, SystemSignature inTarget)
        : world(inWorld)
        , target(std::move(inTarget))
    {
    }

    SystemSchedule::~SystemSchedule() = default;

    SystemSchedule& SystemSchedule::ScheduleAfter(const Mirror::Class& clazz)
    {
        SystemSignature depend = Internal::SignForClass(clazz);
        Assert(world.systemDependencies.contains(target));
        world.systemDependencies.at(target).emplace_back(depend);
        return *this;
    }

    EventSlot::EventSlot(World& inWorld, EventSignature inTarget)
        : world(inWorld)
        , target(std::move(inTarget))
    {
    }

    EventSlot::~EventSlot() = default;

    EventSlot& EventSlot::Connect(const Mirror::Class& clazz)
    {
        SystemSignature systemSignature = Internal::SignForClass(clazz);
        auto object = clazz.GetDefaultConstructor().NewObject();
        auto* systemInstance = clazz.DynamicCast<EventSystem>(&object);
        world.RegisterSystem(systemSignature, systemInstance);
        Assert(world.eventSlots.contains(target));
        world.eventSlots.at(target).emplace_back(systemSignature);
        return *this;
    }

    World::World(std::string inName)
        : ECSHost()
        , name(std::move(inName))
        , setuped(false)
    {
    }

    World::~World() = default;

    void World::BroadcastEvent(EventSignature eventSignature, const Mirror::Any& eventRef)
    {
        if (!eventSlots.contains(eventSignature)) {
            return;
        }

        SystemCommands commands(registry, *this);
        for (const auto& systemId : eventSlots.at(eventSignature)) {
            Assert(systems.contains(systemId));
            auto* system = systems.at(systemId).Get();
            static_cast<EventSystem*>(system)->OnReceiveEvent(commands, eventRef);
        }
    }

    SystemSchedule World::AddSetupSystem(const Mirror::Class& clazz)
    {
        SystemSignature systemSignature = Internal::SignForClass(clazz);
        auto object = clazz.GetDefaultConstructor().NewObject();
        auto* systemInstance = clazz.DynamicCast<SetupSystem>(&object);
        RegisterSystem(systemSignature, systemInstance);
        setupSystems.emplace_back(systemSignature);
        return SystemSchedule(*this, systemSignature);
    }

    SystemSchedule World::AddTickSystem(const Mirror::Class& clazz)
    {
        SystemSignature systemSignature = Internal::SignForClass(clazz);
        auto object = clazz.GetDefaultConstructor().NewObject();
        auto* systemInstance = clazz.DynamicCast<TickSystem>(&object);
        RegisterSystem(systemSignature, systemInstance);
        tickSystems.emplace_back(systemSignature);
        return SystemSchedule(*this, systemSignature);
    }

    EventSlot World::Event(const Mirror::Class& clazz)
    {
        EventSignature eventSignature = Internal::SignForClass(clazz);
        if (!eventSlots.contains(eventSignature)) {
            eventSlots.emplace(std::make_pair(eventSignature, std::vector<EventSignature> {}));
        }
        return EventSlot(*this, eventSignature);
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

    void World::RegisterSystem(const SystemSignature& systemSignature, System* systemInstance)
    {
        Assert(!systems.contains(systemSignature) && !systemDependencies.contains(systemSignature));
        systems.emplace(std::make_pair(systemSignature, Common::UniqueRef<System>(systemInstance)));
        systemDependencies.emplace(std::make_pair(systemSignature, std::vector<SystemSignature> {}));
    }

    void World::ExecuteWorkSystems(const std::vector<SystemSignature>& targets)
    {
        tf::Taskflow taskflow;
        std::unordered_map<SystemSignature, tf::Task> tasks;

        tasks.clear();
        tasks.reserve(targets.size());

        SystemCommands commands(registry, *this);
        for (const auto& target : targets) {
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
