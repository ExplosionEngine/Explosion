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
    class SystemCommands;
    static constexpr auto entityNull = entt::null;

    struct Component {};
    struct GlobalComponent {};
    struct SystemEvent {};
    struct System {};

    template <typename E>
    struct SystemEventDecoder {
    public:
        explicit SystemEventDecoder(const Mirror::Any& inEventRef)
            : eventRef(inEventRef)
        {
        }

        const E& Get()
        {
            return eventRef.CastTo<const E&>();
        }

    private:
        const Mirror::Any& eventRef;
    };

    struct WorkSystem : public System {
        virtual void Execute(SystemCommands& systemCommands) = 0;
    };

    struct SetupSystem : public WorkSystem {};
    struct TickSystem : public WorkSystem {};

    struct EventSystem : public System {
        virtual void OnReceiveEvent(SystemCommands& systemCommands, const Mirror::Any& eventRef) = 0;
    };

    template <typename C>
    struct ComponentAdded : public SystemEvent {
        Entity entity;
    };

    template <typename C>
    struct ComponentUpdated : public SystemEvent {
        Entity entity;
    };

    template <typename C>
    struct ComponentRemoved : public SystemEvent {
        Entity entity;
    };

    template <typename G>
    struct GlobalComponentAdded : public SystemEvent {};

    template <typename G>
    struct GlobalComponentUpdated : public SystemEvent {};

    template <typename C>
    struct GlobalComponentRemoved : public SystemEvent {};

    struct ISystemEventRadio {
    public:
        template <typename E>
        void BroadcastSystemEvent(const E& event)
        {
            Mirror::Any eventRef = std::ref(event);
            BroadcastSystemEvent(Mirror::GetTypeInfo<E>()->id, event);
        }

    protected:
        virtual void BroadcastSystemEvent(Mirror::TypeId eventTypeId, const Mirror::Any& eventRef) = 0;
    };

    template <typename... Args>
    class Query {
    public:
        using Iterable = typename entt::view<Args...>::iterable_view;

        explicit Query(entt::view<Args...>&& inView)
            : view(std::move(inView))
        {
        }

        ~Query() = default;

        template <typename F>
        void Each(F&& func)
        {
            view.each(std::forward<F>(func));
        }

        Iterable Each()
        {
            return view.each();
        }

    private:
        entt::view<Args...> view;
    };

    template <typename... C>
    struct Exclude {};

    class SystemCommands {
    public:
        NonCopyable(SystemCommands)
        SystemCommands(entt::registry& inRegistry, ISystemEventRadio& inSystemEventRadio);
        ~SystemCommands();

        Entity AddEntity();
        void DestroyEntity(Entity inEntity);

        template <typename C, typename... Args>
        void AddComponent(Entity entity, Args&&... args)
        {
            C& component = registry.emplace<C>(entity, std::forward<Args>(args)...);
            Broadcast(ComponentAdded<C> { entity });
        }

        template <typename C>
        const C* GetComponent(Entity entity)
        {
            return registry.try_get<C>(entity);
        }

        template <typename C>
        bool HasComponent(Entity entity)
        {
            return GetComponent<C>(entity) != nullptr;
        }

        template <typename C, typename F>
        void PatchComponent(Entity entity, F&& patchFunc)
        {
            registry.patch<C>(entity, patchFunc);
            Broadcast(ComponentUpdated<C> { entity });
        }

        template <typename C, typename... Args>
        void SetComponent(Entity entity, Args&&... args)
        {
            registry.replace<C>(entity, std::forward<Args>(args)...);
            Broadcast(ComponentUpdated<C> { entity });
        }

        template <typename C>
        void RemoveComponent(Entity entity)
        {
            registry.remove<C>(entity);
            Broadcast(ComponentRemoved<C> { entity });
        }

        template <typename G, typename... Args>
        void AddGlobalComponent(Args&&... args)
        {
            Mirror::TypeId typeId = Mirror::GetTypeInfo<G>()->id;
            auto iter = globalComponents.find(typeId);
            Assert(iter == globalComponents.end());
            globalComponents.emplace(std::make_pair(typeId, Mirror::Any(G(std::forward<Args>(args)...))));
            Broadcast(GlobalComponentAdded<G> {});
        }

        template <typename G>
        const G* GetGlobalComponent()
        {
            Mirror::TypeId typeId = Mirror::GetTypeInfo<G>()->id;
            auto iter = globalComponents.find(typeId);
            if (iter == globalComponents.end()) {
                return nullptr;
            } else {
                return &iter->second.CastTo<const G&>();
            }
        }

        template <typename G>
        bool HasGlobalComponent()
        {
            return GetGlobalComponent<G>() != nullptr;
        }

        template <typename G, typename F>
        void PatchGlobalComponent(F&& patchFunc)
        {
            Mirror::TypeId typeId = Mirror::GetTypeInfo<G>()->id;
            auto iter = globalComponents.find(typeId);
            Assert(iter != globalComponents.end());
            patchFunc(iter->second.CastTo<G&>());
            Broadcast(GlobalComponentUpdated<G> {});
        }

        template <typename G, typename... Args>
        void SetGlobalComponent(Args&&... args)
        {
            Mirror::TypeId typeId = Mirror::GetTypeInfo<G>()->id;
            auto iter = globalComponents.find(typeId);
            Assert(iter != globalComponents.end());
            iter->second = G(std::forward<Args>(args)...);
            Broadcast(GlobalComponentUpdated<G> {});
        }

        template <typename G>
        void RemoveGlobalComponent()
        {
            Mirror::TypeId typeId = Mirror::GetTypeInfo<G>()->id;
            auto iter = globalComponents.find(typeId);
            Assert(iter != globalComponents.end());
            globalComponents.erase(typeId);
            Broadcast(GlobalComponentRemoved<G> {});
        }

        template <typename... C, typename... E>
        Query<entt::exclude_t<E...>, C...> NewQuery(Exclude<E...> = {})
        {
            return Query<entt::exclude_t<E...>, C...>(registry.view<C...>(entt::exclude_t<E...> {}));
        }

        template <typename E>
        void Broadcast(const E& event)
        {
            systemEventRadio.BroadcastSystemEvent<E>(event);
        }

    private:
        ISystemEventRadio& systemEventRadio;
        entt::registry& registry;
        std::unordered_map<Mirror::TypeId, Mirror::Any> globalComponents;
    };
}
