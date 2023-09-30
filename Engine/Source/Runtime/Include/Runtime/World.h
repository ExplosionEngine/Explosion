//
// Created by johnk on 2023/9/5.
//

#pragma once

#include <Common/Memory.h>
#include <Runtime/ECS.h>

namespace Runtime {
    class World;

    class RUNTIME_API SystemSchedule {
    public:
        SystemSchedule(World& inWorld, SystemSignature inTarget);
        ~SystemSchedule();

        template <typename S>
        SystemSchedule& ScheduleAfter();

        template <auto Ptr>
        SystemSchedule& ScheduleAfter();

        SystemSchedule& ScheduleAfter(const std::string& lambdaName);

    private:
        SystemSchedule& ScheduleAfterInternal(SystemSignature depend);

        World& world;
        SystemSignature target;
    };

    class RUNTIME_API EventSlot {
    public:
        EventSlot(World& inWorld, SystemEventSignature inTarget);
        ~EventSlot();

        template <typename S>
        EventSlot& Connect();

        template <auto Ptr>
        EventSlot& Connect();

        EventSlot& Connect(const std::string& lambdaName, const SystemOnReceiveEventFunc& func);

    private:
        World& world;
        SystemEventSignature target;
    };

    class RUNTIME_API World : public ECSHost {
    public:
        explicit World(std::string inName = "");
        ~World();

        template <typename S>
        SystemSchedule AddSetupSystem()
        {
            SystemSignature signature = CreateSystem(Internal::ClassSystemSigner<S>().Sign(), new S());
            setupSystems.emplace_back(signature);
            return SystemSchedule(*this, signature);
        }

        template <typename S>
        SystemSchedule AddTickSystem()
        {
            SystemSignature signature = CreateSystem(Internal::ClassSystemSigner<S>().Sign(), new S());
            tickSystems.emplace_back(signature);
            return SystemSchedule(*this, signature);
        }

        template <auto Ptr>
        SystemSchedule AddSetupSystem()
        {
            SystemSignature signature = CreateSystem(
                Internal::FuncSystemSigner<Ptr>().Sign(),
                new FuncSetupSystem([](SystemCommands& commands) -> void { Ptr(commands); })
            );
            setupSystems.emplace_back(signature);
            return SystemSchedule(*this, signature);
        }

        template <auto Ptr>
        SystemSchedule AddTickSystem()
        {
            SystemSignature signature = CreateSystem(
                Internal::FuncSystemSigner<Ptr>().Sign(),
                new FuncTickSystem([](SystemCommands& commands) -> void { Ptr(commands); })
            );
            tickSystems.emplace_back(signature);
            return SystemSchedule(*this, signature);
        }

        SystemSchedule AddSetupSystem(const std::string& systemName, const SystemExecuteFunc& func);
        SystemSchedule AddTickSystem(const std::string& systemName, const SystemExecuteFunc& func);

        template <typename E>
        EventSlot Event()
        {
            SystemEventSignature signature = Internal::SystemEventSigner<E>().Sign();
            if (!systemEventSlots.contains(signature)) {
                systemEventSlots.emplace(std::make_pair(signature, std::vector<SystemEventSignature> {}));
            }
            return EventSlot(*this, signature);
        }

        void Setup();
        void Shutdown();
        void Tick();

    protected:
        void BroadcastSystemEvent(Mirror::TypeId eventTypeId, const Mirror::Any& eventRef) override;

    private:
        friend class SystemSchedule;
        friend class EventSlot;
        friend class WorldTestHelper;

        SystemSignature CreateSystem(SystemSignature signature, System* systemInstance);
        void ExecuteWorkSystems(const std::vector<SystemSignature>& targets);

        bool setuped;
        std::string name;
        entt::registry registry;
        std::vector<SystemSignature> setupSystems;
        std::vector<SystemSignature> tickSystems;
        std::unordered_map<SystemSignature, Common::UniqueRef<System>> systems;
        std::unordered_map<SystemSignature, std::vector<SystemSignature>> systemDependencies;
        std::unordered_map<SystemEventSignature, std::vector<SystemSignature>> systemEventSlots;
    };

    // used for test only
#if BUILD_TEST
    class RUNTIME_API WorldTestHelper {
    public:
        explicit WorldTestHelper(World& inWorld);
        ~WorldTestHelper();

        SystemCommands HackCreateSystemCommands();

    private:
        World& world;
    };
#endif
}

namespace Runtime {
    template <typename S>
    SystemSchedule& SystemSchedule::ScheduleAfter()
    {
        return ScheduleAfterInternal(Internal::ClassSystemSigner<S>().Sign());
    }

    template <auto Ptr>
    SystemSchedule& SystemSchedule::ScheduleAfter()
    {
        return ScheduleAfterInternal(Internal::FuncSystemSigner<Ptr>().Sign());
    }

    template <typename S>
    EventSlot& EventSlot::Connect()
    {
        SystemSignature system = world.CreateSystem(Internal::ClassSystemSigner<S>().Sign(), new S());
        Assert(world.systemEventSlots.contains(target));
        world.systemEventSlots.at(target).emplace_back(system);
        return *this;
    }

    template <auto Ptr>
    EventSlot& EventSlot::Connect()
    {
        SystemSignature system = world.CreateSystem(
            Internal::FuncSystemSigner<Ptr>().Sign(),
            new FuncTickSystem([](SystemCommands& commands, const Mirror::Any& evnetRef) -> void { Ptr(commands, evnetRef); })
        );
        Assert(world.systemEventSlots.contains(target));
        world.systemEventSlots.at(target).emplace_back(system);
        return *this;
    }
}
