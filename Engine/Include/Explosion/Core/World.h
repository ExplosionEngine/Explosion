//
// Created by Administrator on 2021/4/3 0003.
//

#ifndef EXPLOSION_WORLD_H
#define EXPLOSION_WORLD_H

#include <string>
#include <Explosion/Core/Ecs/Entity.h>
#include <Explosion/Core/Ecs/Component.h>

namespace Explosion {
    class World {
    public:
        World(std::string name);
        ~World();

        Entity CreateEntity();
        void DestroyEntity(const Entity& entity);

        template <typename CompType>
        CompType& AddComponent(const Entity& entity)
        {
            registry.emplace<CompType>(entity);
            return GetComponent<CompType>(entity);
        }

        template <typename CompType>
        CompType& GetComponent(const Entity& entity)
        {
            return registry.get<CompType>(entity);
        }

        template <typename CompType>
        void RemoveComponent(const Entity& entity)
        {
            registry.remove<CompType>(entity);
        }

        template <typename CompType>
        bool HasComponent(const Entity& entity)
        {
            return registry.has<CompType>(entity);
        }

    private:
        std::string name;
        entt::registry registry;
    };
}

#endif //EXPLOSION_WORLD_H
