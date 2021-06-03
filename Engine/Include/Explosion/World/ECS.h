//
// Created by John Kindem on 2021/6/3.
//

#ifndef EXPLOSION_ECS_H
#define EXPLOSION_ECS_H

#include <entt/entt.hpp>

namespace Explosion::ECS {
    using Entity = entt::entity;

    struct Component {};

    class Registry {
    public:
        Registry() = default;
        ~Registry() = default;

        Entity CreateEntity()
        {
            return registry.create();
        }

        void DestroyEntity(Entity entity)
        {
            registry.destroy(entity);
        }

        template <typename Comp, typename... Args>
        void AddComponent(Entity entity, const Args&... args)
        {
            registry.template emplace_or_replace<Comp>(entity, args...);
        }

        template <typename Comp>
        Comp& GetComponent(Entity entity)
        {
            registry.template get<Comp>(entity);
        }

    private:
        entt::registry registry;
    };

    class System {
    public:
        explicit System(Registry& registry) : registry(registry) {}
        ~System() = default;

    private:
        Registry& registry;
    };
}

#endif //EXPLOSION_ECS_H
