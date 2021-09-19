//
// Created by John Kindem on 2021/6/10.
//

#ifndef EXPLOSION_ECS_H
#define EXPLOSION_ECS_H

#include <memory>

#include <entt/entt.hpp>

#include <JobSystem/JobSystem.h>

namespace Explosion::ECS {
    using Entity = entt::entity;
    constexpr Entity INVALID_ENTITY = Entity{entt::entt_traits<Entity>::entity_mask};

    class Registry;

    template <typename... T>
    using ExcludeT = entt::exclude_t<T...>;

    template <typename... T>
    class View {
    public:
        ~View() = default;

        template <typename Func>
        void Each(const Func& func)
        {
            return view.each(func);
        }

    private:
        friend Registry;

        explicit View(entt::view<T...> view) : view(std::move(view)) {}

        entt::view<T...> view;
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

        bool IsActive(Entity entity) const
        {
            return registry.valid(entity);
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

        template <typename Proxy>
        Proxy CreateProxy()
        {
            return Proxy(*this);
        }

    private:
        entt::registry registry;
    };

    using System = std::function<void(Registry& registry, float time)>;

    struct SystemGroup {
        std::string name;
        uint32_t priority;
        std::unordered_map<std::string, System> systems;
        std::unordered_map<std::string, std::string> dependencies;
    };
}

#endif //EXPLOSION_ECS_H
