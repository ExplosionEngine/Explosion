//
// Created by johnk on 2022/8/10.
//

#pragma once

#include <entt/entt.hpp>
#include <Mirror/Meta.h>

namespace Runtime {
    class World;

    using Entity = entt::entity;
    static constexpr auto entityNull = entt::null;

    class EClass() Component {
    public:
        EClassBody(Component)

        ECtor()
        Component() = default;
    };

    template <typename... Components>
    struct ComponentSet {};

    template <typename... Components>
    class Query {
    public:
        using ComponentSet = ComponentSet<Components...>;

        explicit Query(entt::view<entt::exclude_t<>, Components...>&& inView) : view(inView) {}
        ~Query() = default;

        template <typename F>
        void ForEach(F&& func) const
        {
            view.each(std::forward<F>(func));
        }

    private:
        entt::view<entt::exclude_t<>, Components...> view;
    };

    template <typename T>
    struct SystemFuncTraits {};

    template <typename Class, typename... Queries>
    struct SystemFuncTraits<void(Class::*)(const Queries&...)> {
        using QueryTuple = std::tuple<Queries...>;
    };

    template <typename T>
    struct QueryTraits {};

    template <typename... Components>
    struct QueryTraits<Query<Components...>> {
        using ComponentSet = ComponentSet<Components...>;
    };

    class System {
    public:
        virtual ~System() = default;

        void Wait(System* systemToWait)
        {
            if (systemToWait == nullptr) {
                return;
            }
            systemsToWait.emplace_back(systemToWait);
        }

        void Wait(const std::vector<System*>& inSystemsToWait)
        {
            for (auto* systemToWait : inSystemsToWait) {
                if (systemToWait == nullptr) {
                    continue;
                }
                systemsToWait.emplace_back(systemToWait);
            }
        }

        [[nodiscard]] const std::vector<System*>& GetSystemsToWait()
        {
            return systemsToWait;
        }

    protected:
        System() = default;

    private:
        std::vector<System*> systemsToWait;
    };
}
