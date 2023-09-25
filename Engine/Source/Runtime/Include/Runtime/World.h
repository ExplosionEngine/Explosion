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
        SystemSchedule();
        ~SystemSchedule();

        template <typename S>
        SystemSchedule& ScheduleAfter();

        template <auto Ptr>
        SystemSchedule& ScheduleAfter();

        SystemSchedule& ScheduleAfter(const std::string& lambdaName);

    private:
        friend class World;

        SystemSchedule& ScheduleAfterInternal(SystemSignature depend);
        void InvokeSchedule(World& world, SystemSignature target) const;

        std::vector<std::function<void(World&, SystemSignature)>> scheduleFuncs;
    };

    class RUNTIME_API EventSlot {
    public:
        EventSlot();
        ~EventSlot();

        template <typename S>
        EventSlot& Connect();

        template <auto Ptr>
        EventSlot& Connect();

        EventSlot& Connect(const std::string& lambdaName, const SystemOnReceiveEventFunc& func);


    private:
        friend class World;

        void InvokeConnect(World& world, SystemEventSignature target) const;

        std::vector<std::function<void(World&, SystemEventSignature)>> connectFuncs;
    };

    class RUNTIME_API World : public ECSHost {
    public:
        explicit World(std::string inName = "");
        ~World();

        template <typename S>
        World& AddSetupSystem(const SystemSchedule& schedule = {})
        {
            SystemSignature signature = CreateSystem(Internal::ClassSystemSigner<S>().Sign(), new S());
            schedule.InvokeSchedule(*this, setupSystems.emplace_back(signature));
            return *this;
        }

        template <typename S>
        World& AddTickSystem(const SystemSchedule& schedule = {})
        {
            SystemSignature signature = CreateSystem(Internal::ClassSystemSigner<S>().Sign(), new S());
            schedule.InvokeSchedule(*this, tickSystems.emplace_back(signature));
            return *this;
        }

        template <auto Ptr>
        World& AddSetupSystem(const SystemSchedule& schedule = {})
        {
            SystemSignature signature = CreateSystem(
                Internal::FuncSystemSigner<Ptr>().Sign(),
                new FuncSetupSystem([](SystemCommands& systemCommands) -> void { Ptr(systemCommands); })
            );
            schedule.InvokeSchedule(*this, setupSystems.emplace_back(signature));
            return *this;
        }

        template <auto Ptr>
        World& AddTickSystem(const SystemSchedule& schedule = {})
        {
            SystemSignature signature = CreateSystem(
                Internal::FuncSystemSigner<Ptr>().Sign(),
                new FuncTickSystem([](SystemCommands& systemCommands) -> void { Ptr(systemCommands); })
            );
            schedule.InvokeSchedule(*this, tickSystems.emplace_back(signature));
            return *this;
        }

        World& AddSetupSystem(const std::string& systemName, const SystemExecuteFunc& func, const SystemSchedule& schedule = {});
        World& AddTickSystem(const std::string& systemName, const SystemExecuteFunc& func, const SystemSchedule& schedule = {});

        template <typename E>
        World& RegisterEvent(const EventSlot& eventSlot = {})
        {
            SystemEventSignature signature = Internal::SystemEventSigner<E>().Sign();
            Assert(!systemEventSlots.contains(signature));
            systemEventSlots.emplace(std::make_pair(signature, std::vector<SystemEventSignature> {}));
            eventSlot.InvokeConnect(*this, signature);
            return *this;
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
        if constexpr (std::is_function_v<std::remove_reference_t<std::remove_pointer_t<decltype(Ptr)>>>) {
            return ScheduleAfterInternal(Internal::FuncSystemSigner<Ptr>().Sign());
        } else {
            static_assert(false, "bad func pointer type parameter");
        }
    }

    template <typename S>
    EventSlot& EventSlot::Connect()
    {
        connectFuncs.emplace_back([](World& world, SystemEventSignature target) -> void {
            SystemSignature system = world.CreateSystem(Internal::ClassSystemSigner<S>().Sign(), new S());
            Assert(world.systemEventSlots.contains(target));
            world.systemEventSlots.at(target).emplace_back(system);
        });
        return *this;
    }

    template <auto Ptr>
    EventSlot& EventSlot::Connect()
    {
        connectFuncs.emplace_back([](World& world, SystemEventSignature target) -> void {
            SystemSignature system = world.CreateSystem(
                Internal::FuncSystemSigner<Ptr>().Sign(),
                new FuncTickSystem([](SystemCommands& systemCommands, const Mirror::Any& evnetRef) -> void { Ptr(systemCommands, evnetRef); })
            );
            Assert(world.systemEventSlots.contains(target));
            world.systemEventSlots.at(target).emplace_back(system);
        });
        return *this;
    }
}
