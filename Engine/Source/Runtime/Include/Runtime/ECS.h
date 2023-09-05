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

    struct Component {};
    struct GlobalComponent {};
    struct System {};
    struct SystemEvent {};

    template <typename S>
    struct SystemSchedule {
        template <typename DS>
        SystemSchedule<S> Depend()
        {
            dependencies.emplace_back(Mirror::GetTypeInfo<DS>()->id);
            return *this;
        }

        std::vector<Mirror::TypeId> dependencies;
    };

    template <typename E>
    struct EventSlot {
        template <typename S>
        EventSlot<E> Connect()
        {
            systems.emplace_back(Mirror::GetTypeInfo<S>()->id);
            return *this;
        }

        std::vector<Mirror::TypeId> systems;
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
        void Broadcast(const E& event)
        {
            Mirror::Any eventRef = std::ref(event);
            BroadcastSystemEvent(Mirror::GetTypeInfo<E>()->id, event);
        }

    protected:
        virtual void BroadcastSystemEvent(Mirror::TypeId eventTypeId, Mirror::Any eventRef) = 0;
    };

    template <typename... C>
    class Query {
    public:
        // TODO
    };

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

        template <typename... C>
        Query<C...> NewQuery()
        {
            // TODO
        }

        template <typename E>
        void Broadcast(const E& event)
        {
            systemEventRadio.Broadcast<E>(event);
        }

    private:
        ISystemEventRadio& systemEventRadio;
        entt::registry& registry;
        std::unordered_map<Mirror::TypeId, Mirror::Any> globalComponents;
    };
}
