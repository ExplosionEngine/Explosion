//
// Created by John Kindem on 2021/6/3.
//

#ifndef EXPLOSION_ECS_H
#define EXPLOSION_ECS_H

#include <functional>

#include <entt/entt.hpp>

namespace Explosion::ECS {
    using Entity = entt::entity;

    struct Component {};

    template <typename... Args>
    class View {
    public:
        explicit View(entt::view<Args...>& view) : view(view) {}
        ~View() = default;

        void Each(const std::function<void(const Args&...)>& func)
        {
            view.template each(func);
        }

        void Each(const std::function<void(Entity entity, const Args&...)>& func)
        {
            view.template each(func);
        }

    private:
        entt::view<Args...>& view;
    };

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

        template <typename... Args>
        View<Args...> View()
        {
            return registry.template view<Args...>();
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
