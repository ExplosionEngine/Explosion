//
// Created by johnk on 2022/8/10.
//

#pragma once

#include <entt/entt.hpp>
#include <Mirror/Meta.h>
#include <Runtime/Api.h>

#define DefineWaitSystemTypes(...) using WaitSystemTypes = std::tuple<__VA_ARGS__>;

namespace Runtime {
    class World;

    using Entity = entt::entity;
    static constexpr auto entityNull = entt::null;

    class RUNTIME_API EClass() Component {
    public:
        EClassBody(Component)

        ECtor()
        Component() = default;

        virtual ~Component() = default;

    protected:
        friend class World;

        virtual void OnConstruct() {}
        virtual void OnDestroy() {}

        World* world;
        Entity entity;
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

    class EClass() System {
    public:
        EClassBody(System)

        virtual ~System() = default;

    protected:
        friend class World;

        System() = default;
    };
}
