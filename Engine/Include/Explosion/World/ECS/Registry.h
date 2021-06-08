//
// Created by John Kindem on 2021/6/8.
//

#ifndef EXPLOSION_REGISTRY_H
#define EXPLOSION_REGISTRY_H

#include <Explosion/World/ECS/Entity.h>
#include <Explosion/World/ECS/View.h>

namespace Explosion::ECS {
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
        void AddComponent(Entity entity, Args&&... args)
        {
            registry.template emplace_or_replace<Comp>(entity, args...);
        }

        template <typename Comp>
        Comp* GetComponent(Entity entity)
        {
            return registry.template try_get<Comp>(entity);
        }

        template <typename... Comp, typename... Exclude>
        View<ExcludeT<Exclude...>, Comp...> CreateView(const ExcludeT<Exclude...>& excludes = {})
        {
            return View<ExcludeT<Exclude...>, Comp...>(registry.template view<Comp...>(excludes));
        }

    private:
        entt::registry registry;
    };
}

#endif //EXPLOSION_REGISTRY_H
