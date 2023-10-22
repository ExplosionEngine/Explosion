//
// Created by johnk on 2022/8/10.
//

#pragma once

#include <entt/entt.hpp>

#include <Common/Utility.h>
#include <Common/Hash.h>
#include <Common/String.h>
#include <Mirror/Meta.h>
#include <Mirror/Type.h>
#include <Runtime/Api.h>

#define DeclareSingleCompLifecycleEvent(eventClass) \
    struct EClass() eventClass : public Runtime::Event { \
        EClassBody(eventClass) \
        \
        EProperty() \
        Entity entity; \
    }; \

#define DeclareCompLifecycleEvents() \
    DeclareSingleCompLifecycleEvent(Added) \
    DeclareSingleCompLifecycleEvent(Updated) \
    DeclareSingleCompLifecycleEvent(Removed) \

#define EComponentBody(compClass) \
    EClassBody(compClass) \
    DeclareCompLifecycleEvents() \

#define DeclareSingleStateLifecycleEvent(eventClass) \
    struct EClass() eventClass : public Runtime::Event { \
        EClassBody(eventClass) \
    }; \

#define DeclareStateLifecycleEvents() \
    DeclareSingleStateLifecycleEvent(Added) \
    DeclareSingleStateLifecycleEvent(Updated) \
    DeclareSingleStateLifecycleEvent(Removed) \

#define EStateBody(stateClass) \
    EClassBody(stateClass) \
    DeclareStateLifecycleEvents() \

namespace Runtime {
    struct ClassSignature {
        size_t typeId;
        std::string name;

        bool operator==(const ClassSignature& rhs) const
        {
            return typeId == rhs.typeId;
        }
    };

    using ComponentSignature = ClassSignature;
    using StateSignature = ClassSignature;
    using SystemSignature = ClassSignature;
    using EventSignature = ClassSignature;
}

namespace std {
    template <>
    struct hash<Runtime::ClassSignature> {
        size_t operator()(const Runtime::ClassSignature& value) const
        {
            return std::hash<size_t>{}(value.typeId);
        }
    };
}

namespace Runtime::Internal {
    template <typename C>
    SystemSignature SignForClass()
    {
        const Mirror::Class& clazz = C::GetClass();

        SystemSignature result;
        result.typeId = clazz.GetTypeInfo()->id;
        result.name = clazz.GetName();
        return result;
    }
}

namespace Runtime {
    using Entity = entt::entity;
    class SystemCommands;
    static constexpr auto entityNull = entt::null;

    struct EClass() Component {
        EClassBody(Component)
    };

    struct EClass() State {
        EClassBody(State)
    };

    struct EClass() Event {
        EClassBody(Event)
    };

    struct EClass() System {
        EClassBody(System)
    };

    enum class SystemType : uint8_t {
        setup,
        tick,
        event,
        max
    };

    struct ECSHost {
    public:
        template <typename S>
        void AddSetupSystem()
        {
            // TODO
        }

        template <typename S>
        void AddTickSystem()
        {
            // TODO
        }

        template <typename E, typename S>
        void AddEventSystem()
        {
            // TODO
        }

        template <typename S>
        void RemoveSetupSystem()
        {
            // TODO
        }

        template <typename S>
        void RemoveTickSystem()
        {
            // TODO
        }

        template <typename E, typename S>
        void RemoveEventSystem()
        {
            // TODO
        }

        template <typename E>
        void BroadcastEvent(const E& event)
        {
            // TODO
        }

    protected:
        virtual void Setup()
        {
            // TODO
        }

        virtual void Tick(float timeMS)
        {
            // TODO
        }

        virtual void Shutdown()
        {
            // TODO
        }

        ECSHost() = default;

        struct SystemInstance {
            SystemType type;
            Common::UniqueRef<System> object;
            union {
                std::function<void(SystemCommands&)> setupFunc;
                std::function<void(SystemCommands&, float timeMS)> tickFunc;
                std::function<void(SystemCommands&, Mirror::Any*)> onReceiveEventFunc;
            };
        };

        bool setuped;
        entt::registry registry;
        std::unordered_map<SystemSignature, SystemInstance> systemInstances;
        std::vector<SystemSignature> setupSystems;
        std::vector<SystemSignature> tickSystems;
        std::unordered_map<EventSignature, std::vector<SystemSignature>> eventSystems;
        std::unordered_map<SystemSignature, std::vector<SystemSignature>> setupSystemDependencies;
        std::unordered_map<SystemSignature, std::vector<SystemSignature>> tickSystemDependencies;
        std::unordered_map<EventSignature, std::unordered_map<SystemSignature, std::vector<SystemSignature>>> eventSystemDependencies;
        std::unordered_map<StateSignature, Mirror::Any> states;

    private:
        friend class SystemCommands;
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

        auto Each()
        {
            return view.each();
        }

    private:
        entt::view<Args...> view;
    };

    template <typename... C>
    struct Exclude {};

    class SystemCommands;

    class RUNTIME_API SystemCommands {
    public:
        NonCopyable(SystemCommands)
        SystemCommands(entt::registry& inRegistry, ECSHost& inHost);
        ~SystemCommands();

        Entity Create(Entity hint = entityNull);
        void Destroy(Entity inEntity);
        bool Valid(Entity inEntity) const;

        template <typename C, typename... Args>
        void Emplace(Entity entity, Args&&... args)
        {
            registry.emplace<C>(entity, std::forward<Args>(args)...);
            Broadcast(typename C::Added { {}, entity });
        }

        template <typename C>
        C* Get(Entity entity)
        {
            return registry.try_get<C>(entity);
        }

        template <typename C>
        bool Has(Entity entity)
        {
            return Get<C>(entity) != nullptr;
        }

        template <typename C, typename F>
        void Patch(Entity entity, F&& patchFunc)
        {
            registry.patch<C>(entity, patchFunc);
            Broadcast(typename C::Updated { {}, entity });
        }

        template <typename C, typename... Args>
        void Set(Entity entity, Args&&... args)
        {
            registry.replace<C>(entity, std::forward<Args>(args)...);
            Broadcast(typename C::Updated { {}, entity });
        }

        template <typename C>
        void Updated(Entity entity)
        {
            Broadcast(typename C::Updated { {}, entity });
        }

        template <typename C>
        void Remove(Entity entity)
        {
            registry.remove<C>(entity);
            Broadcast(typename C::Removed { {}, entity });
        }

        template <typename S, typename... Args>
        void EmplaceState(Args&&... args)
        {
            StateSignature signature = Internal::SignForClass(S::GetClass());
            auto iter = host.states.find(signature);
            Assert(iter == host.states.end());
            host.states.emplace(std::make_pair(signature, Mirror::Any(S(std::forward<Args>(args)...))));
            Broadcast(typename S::Added {});
        }

        template <typename S>
        S* GetState()
        {
            StateSignature signature = Internal::SignForClass(S::GetClass());
            auto iter = host.states.find(signature);
            if (iter == host.states.end()) {
                return nullptr;
            } else {
                return &iter->second.As<S&>();
            }
        }

        template <typename S>
        bool HasState()
        {
            return GetState<S>() != nullptr;
        }

        template <typename S, typename F>
        void PatchState(F&& patchFunc)
        {
            StateSignature signature = Internal::SignForClass(S::GetClass());
            auto iter = host.states.find(signature);
            Assert(iter != host.states.end());
            patchFunc(iter->second.As<S&>());
            Broadcast(typename S::Updated {});
        }

        template <typename S, typename... Args>
        void SetState(Args&&... args)
        {
            StateSignature signature = Internal::SignForClass(S::GetClass());
            auto iter = host.states.find(signature);
            Assert(iter != host.states.end());
            iter->second = S(std::forward<Args>(args)...);
            Broadcast(typename S::Updated {});
        }

        template <typename S>
        void UpdatedState()
        {
            Broadcast(typename S::Updated {});
        }

        template <typename S>
        void RemoveState()
        {
            StateSignature signature = Internal::SignForClass(S::GetClass());
            auto iter = host.states.find(signature);
            Assert(iter != host.states.end());
            host.states.erase(signature);
            Broadcast(typename S::Removed {});
        }

        template <typename... C, typename... E>
        Query<entt::exclude_t<E...>, C...> StartQuery(Exclude<E...> = {})
        {
            return Query<entt::exclude_t<E...>, C...>(registry.view<C...>(entt::exclude_t<E...> {}));
        }

        template <typename E>
        void Broadcast(const E& event)
        {
            host.BroadcastEvent<E>(event);
        }

    private:
        ECSHost& host;
        entt::registry& registry;
    };
}
