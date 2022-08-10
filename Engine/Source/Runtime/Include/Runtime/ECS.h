//
// Created by kindemhuang on 2022/8/10.
//

#pragma once

#include <entt/entt.hpp>

namespace Runtime {
    using Entity = entt::entity;

    struct Component {};

    template <typename... Components>
    struct ComponentSet {};

    template <typename... Components>
    class Query {
    public:
        using ComponentSet = ComponentSet<Components...>;

        explicit Query(entt::view<Components...>&& inView) : view(inView) {}
        ~Query() = default;

        template <typename F>
        void ForEach(F&& func)
        {
            view.each(std::forward<F>(func));
        }

    private:
        entt::view<Components...> view;
    };

    template <typename T>
    struct SystemFuncTraits {};

    template <typename Class, typename... Queries>
    struct SystemFuncTraits<void(Class::*)(Queries&&...)> {
        using QueryTuple = std::tuple<Queries...>;
    };

    class System {
    public:
        virtual ~System() = default;

        void Wait(System* systemToWait)
        {
            systemsToWait.emplace_back(systemToWait);
        }

        std::vector<System*> GetSystemsToWait()
        {
            return systemsToWait;
        }

    protected:
        System() = default;

    private:
        std::vector<System*> systemsToWait;
    };
}
