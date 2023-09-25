//
// Created by johnk on 2023/9/5.
//

#include <taskflow/taskflow.hpp>

#include <Runtime/World.h>

namespace Runtime {
    SystemSchedule::SystemSchedule() = default;

    SystemSchedule::~SystemSchedule() = default;

    SystemSchedule& SystemSchedule::ScheduleAfter(const std::string& lambdaName)
    {
        return ScheduleAfterInternal(Internal::LambdaSystemSigner(lambdaName).Sign());
    }

    SystemSchedule& SystemSchedule::ScheduleAfterInternal(SystemSignature depend)
    {
        scheduleFuncs.emplace_back([depend](World& world, SystemSignature target) -> void {
            Assert(world.systemDependencies.contains(target));
            world.systemDependencies.at(target).emplace_back(depend);
        });
        return *this;
    }

    void SystemSchedule::InvokeSchedule(World& world, SystemSignature target) const
    {
        for (const auto& func : scheduleFuncs) {
            func(world, target);
        }
    }

    EventSlot::EventSlot() = default;

    EventSlot::~EventSlot() = default;

    EventSlot& EventSlot::Connect(const std::string& lambdaName, const SystemOnReceiveEventFunc& func)
    {
        connectFuncs.emplace_back([lambdaName, func](World& world, SystemEventSignature target) -> void {
            SystemSignature system = world.CreateSystem(Internal::LambdaSystemSigner(lambdaName).Sign(), new FuncEventSystem(func));
            Assert(world.systemEventSlots.contains(target));
            world.systemEventSlots.at(target).emplace_back(system);
        });
        return *this;
    }

    void EventSlot::InvokeConnect(World& world, SystemEventSignature target) const
    {
        for (const auto& func : connectFuncs) {
            func(world, target);
        }
    }

    World::World(std::string inName)
        : ECSHost()
        , name(std::move(inName))
        , setuped(false)
    {
    }

    World::~World() = default;

    void World::BroadcastSystemEvent(Mirror::TypeId eventTypeId, const Mirror::Any& eventRef)
    {
        SystemEventSignature signature = eventTypeId;

        if (!systemEventSlots.contains(signature)) {
            return;
        }

        SystemCommands systemCommands(registry, *this);
        for (const auto& systemId : systemEventSlots.at(signature)) {
            Assert(systems.contains(systemId));
            auto* system = systems.at(systemId).Get();
            static_cast<EventSystem*>(system)->OnReceiveEvent(systemCommands, eventRef);
        }
    }

    World& World::AddSetupSystem(const std::string& systemName, const SystemExecuteFunc& func, const SystemSchedule& schedule)
    {
        SystemSignature signature = CreateSystem(Internal::LambdaSystemSigner(systemName).Sign(), new FuncSetupSystem(func));
        schedule.InvokeSchedule(*this, setupSystems.emplace_back(signature));
        return *this;
    }

    World& World::AddTickSystem(const std::string& systemName, const SystemExecuteFunc& func, const SystemSchedule& schedule)
    {
        SystemSignature signature = CreateSystem(Internal::LambdaSystemSigner(systemName).Sign(), new FuncTickSystem(func));
        schedule.InvokeSchedule(*this, tickSystems.emplace_back(signature));
        return *this;
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

        SystemCommands systemCommands(registry, *this);
        for (SystemSignature target : targets) {
            tasks.emplace(std::make_pair(target, taskflow.emplace([this, &systemCommands, target]() -> void {
                Assert(systems.contains(target));
                auto* system = systems.at(target).Get();
                static_cast<WorkSystem*>(system)->Execute(systemCommands);
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
