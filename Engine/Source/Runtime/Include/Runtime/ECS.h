//
// Created by johnk on 2022/8/10.
//

#pragma once

#include <entt/entt.hpp>

#include <Common/Utility.h>
#include <Mirror/Meta.h>
#include <Mirror/Type.h>
#include <Runtime/Api.h>

namespace Runtime {
    using Entity = entt::entity;
    static constexpr auto entityNull = entt::null;

    class SystemCommands;

    struct EClass() Component {
        EClassBody(Component)
    };

    struct EClass() GlobalComponent {
        EClassBody(GlobalComponent)
    };

    struct EClass() System {
        EClassBody(System)
    };

    struct EClass() Event {
        EClassBody(Event)
    };

    template <typename C>
    struct Added : public Event {
        Entity entity;
        C& component;
    };

    template <typename C>
    class Updated : public Event {
        Entity entity;
        C& component;
    };

    template <typename C>
    class Destroy : public Event {
        Entity entity;
        C& component;
    };

    template <typename... C>
    class Query {
    public:
        // TODO
    };

    class SystemCommands {
    public:
        NonCopyable(SystemCommands) explicit SystemCommands(entt::registry& inRegistry);
        ~SystemCommands();

        Entity AddEntity();
        void DestroyEntity(Entity inEntity);

        template <typename C, typename... Args>
        void AddComponent(Entity entity, Args&&... args)
        {
            // TODO
        }

        template <typename C>
        const C* GetComponent(Entity entity)
        {
            // TODO
        }

        template <typename C, typename F>
        void PatchComponent(Entity entity, F&& patchFunc)
        {
            // TODO
        }

        template <typename C>
        void SetComponent(Entity entity, C&& component)
        {
            // TODO
        }

        template <typename C>
        void RemoveComponent(Entity entity)
        {
            // TODO
        }

        template <typename G, typename... Args>
        void AddGlobalComponent(Args&&... args)
        {
            // TODO
        }

        template <typename G>
        const G& GetGlobalComponent()
        {
            // TODO
        }

        template <typename G, typename F>
        void PatchGlobalComponent(F&& patchFunc)
        {
            // TODO
        }

        template <typename G>
        void SetGlobalComponent(G&& globalComponent)
        {
            // TODO
        }

        template <typename G>
        void RemoveGlobalComponent()
        {
            // TODO
        }

        template <typename... C>
        Query<C...> NewQuery()
        {
            // TODO
        }

        template <typename E>
        void Broadcast(E&& event)
        {
            // TODO
        }

    private:
        entt::registry& registry;
    };
}
