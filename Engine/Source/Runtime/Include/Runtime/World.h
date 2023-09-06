//
// Created by johnk on 2023/9/5.
//

#pragma once

#include <Common/Memory.h>
#include <Runtime/ECS.h>

namespace Runtime {
    class World : public ISystemEventRadio {
    public:
        explicit World(std::string inName = "");
        ~World();

        template <typename S>
        World& AddSystem(const SystemSchedule<S>& schedule)
        {
            // TODO
        }

        template <typename E>
        World& AddEventSlot(const EventSlot<E>& eventSlot)
        {
            // TODO
        }

    protected:
        void BroadcastSystemEvent(Mirror::TypeId eventTypeId, Mirror::Any eventRef) override;

    private:
        std::unordered_map<Mirror::TypeId, Common::UniqueRef<System>> systems;
        std::unordered_map<Mirror::TypeId, std::vector<Mirror::TypeId>> systemDependencies;
        std::unordered_map<Mirror::TypeId, std::vector<Mirror::TypeId>> systemEventSlots;
        std::string name;
        entt::registry registry;
    };
}
