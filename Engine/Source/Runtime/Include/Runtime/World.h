//
// Created by johnk on 2023/9/5.
//

#pragma once

#include <Common/Memory.h>
#include <Runtime/ECS.h>

namespace Runtime {
    using SystemTypeId = Mirror::TypeId;
    using SystemEventTypeId = Mirror::TypeId;
    class World;

    class SystemSchedule {
    public:
        SystemSchedule();
        ~SystemSchedule();

        template <typename S>
        SystemSchedule& ScheduleAfter();

    private:
        friend class World;

        void InvokeSchedule(World& world, SystemTypeId target) const;

        std::vector<std::function<void(World&, SystemTypeId)>> scheduleFuncs;
    };

    class EventSlot {
    public:
        EventSlot();
        ~EventSlot();

        template <typename S>
        EventSlot& Connect();

    private:
        friend class World;

        void InvokeConnect(World& world, SystemEventTypeId target) const;

        std::vector<std::function<void(World&, SystemEventTypeId)>> connectFuncs;
    };

    class World : public ISystemEventRadio {
    public:
        explicit World(std::string inName = "");
        ~World();

        template <typename S>
        World& AddSetupSystem(const SystemSchedule& schedule = {})
        {
            SystemTypeId typeId = CreateSystemGeneral<S>();
            schedule.InvokeSchedule(*this, setupSystems.emplace_back(typeId));
            return *this;
        }

        template <typename S>
        World& AddTickSystem(const SystemSchedule& schedule = {})
        {
            SystemTypeId typeId = CreateSystemGeneral<S>();
            schedule.InvokeSchedule(*this, tickSystems.emplace_back(typeId));
            return *this;
        }

        template <typename E>
        World& RegisterEvent(const EventSlot& eventSlot = {})
        {
            SystemEventTypeId typeId = Mirror::GetTypeInfo<E>()->id;
            Assert(!systemEventSlots.contains(typeId));
            systemEventSlots.emplace(std::make_pair(typeId, std::vector<SystemTypeId> {}));
            eventSlot.InvokeConnect(*this, typeId);
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

        void ExecuteSystems(const std::vector<SystemTypeId>& targets);

        template <typename S>
        SystemTypeId CreateSystemGeneral()
        {
            SystemTypeId typeId = Mirror::GetTypeInfo<S>()->id;
            Assert(!systems.contains(typeId) && !systemDependencies.contains(typeId));
            systems.emplace(std::make_pair(typeId, Common::MakeUnique<S>()));
            systemDependencies.emplace(std::make_pair(typeId, std::vector<SystemTypeId> {}));
            return typeId;
        }

        bool setuped;
        std::string name;
        entt::registry registry;
        std::vector<SystemTypeId> setupSystems;
        std::vector<SystemTypeId> tickSystems;
        std::unordered_map<SystemTypeId, Common::UniqueRef<System>> systems;
        std::unordered_map<SystemTypeId, std::vector<SystemTypeId>> systemDependencies;
        std::unordered_map<SystemEventTypeId, std::vector<SystemTypeId>> systemEventSlots;
    };
}

namespace Runtime {
    template <typename S>
    SystemSchedule& SystemSchedule::ScheduleAfter()
    {
        scheduleFuncs.emplace_back([](World& world, SystemTypeId target) -> void {
            SystemTypeId dependId = Mirror::GetTypeInfo<S>()->id;
            Assert(world.systemDependencies.contains(target));
            world.systemDependencies.at(target).emplace_back(dependId);
        });
        return *this;
    }

    template <typename S>
    EventSlot& EventSlot::Connect()
    {
        connectFuncs.emplace_back([](World& world, SystemEventTypeId target) -> void {
            SystemTypeId systemId = world.CreateSystemGeneral<S>();
            Assert(world.systemEventSlots.contains(target));
            world.systemEventSlots.at(target).emplace_back(systemId);
        });
        return *this;
    }
}
