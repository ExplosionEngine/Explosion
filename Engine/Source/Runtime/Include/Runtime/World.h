//
// Created by johnk on 2022/8/8.
//

#pragma once

#include <entt/entt.hpp>
#include <utility>

#include <Common/Utility.h>
#include <Runtime/Entity.h>

namespace Runtime {
    class World {
    public:
        NON_COPYABLE(World)
        explicit World(std::string inName) : name(std::move(inName)) {}
        ~World() = default;

        Entity CreateEntity()
        {
            return registry.create();
        }

        void DestroyEntity(Entity entity)
        {
            registry.destroy(entity);
        }

        template <typename T, typename... Args>
        T& EmplaceComponent(Entity entity, Args&&... args)
        {
            return registry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
        }

        template <typename T>
        T* GetComponent(Entity entity)
        {
            return registry.get<T>(entity);
        }

        template <typename T>
        void RemoveComponent(Entity entity)
        {
            registry.remove<T>(entity);
        }

    private:
        std::string name;
        entt::registry registry;
    };
}
