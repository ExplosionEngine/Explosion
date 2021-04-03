//
// Created by Administrator on 2021/4/3 0003.
//

#ifndef EXPLOSION_REGISTRY_H
#define EXPLOSION_REGISTRY_H

#include <Explosion/Core/Ecs/Wrapper.h>
#include <Explosion/Core/Ecs/Entity.h>
#include <Explosion/Core/Ecs/Component.h>

namespace Explosion {
    class Registry {
    public:
        Registry(entt::registry& registry);
        ~Registry();

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

        template <typename... Args>
        CompView<Args...> ComponentView()
        {
            return CompView<Args...>(registry.view<Args...>());
        }

    private:
        entt::registry& registry;
    };
}

#endif //EXPLOSION_REGISTRY_H
