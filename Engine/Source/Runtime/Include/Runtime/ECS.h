//
// Created by johnk on 2022/8/10.
//

#pragma once

#include <entt/entt.hpp>

#include <Common/Utility.h>
#include <Common/Hash.h>
#include <Mirror/Meta.h>
#include <Mirror/Type.h>
#include <Runtime/Api.h>

namespace Runtime {
    struct ClassSignature {
        bool canReflect;
        size_t typeId;
        std::string name;

        bool operator==(const ClassSignature& rhs) const
        {
            return typeId == rhs.typeId;
        }
    };

    using ComponentSignature = ClassSignature;
    using GlobalComponentSignature = ClassSignature;
    using SystemSignature = ClassSignature;
    using SystemEventSignature = ClassSignature;
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
    template <typename T>
    SystemSignature SignForClass()
    {
        static SystemSignature signature = []() -> SystemSignature {
            const Mirror::Class* clazz = Mirror::Class::Find<T>();

            SystemSignature result;
            result.canReflect = clazz != nullptr;
            result.typeId = Mirror::GetTypeInfo<T>()->id;
            result.name = clazz != nullptr ? clazz->GetName() : "";
            return result;
        }();
        return signature;
    }
}

namespace Runtime {
    using Entity = entt::entity;
    class SystemCommands;
    static constexpr auto entityNull = entt::null;

    struct Component {};
    struct GlobalComponent {};
    struct SystemEvent {};

    struct System {
        virtual ~System() = default;
    };

    template <typename E>
    struct SystemEventDecoder {
    public:
        explicit SystemEventDecoder(const Mirror::Any& inEventRef)
            : eventRef(inEventRef)
        {
        }

        const E& Get()
        {
            return eventRef.As<const E&>();
        }

    private:
        const Mirror::Any& eventRef;
    };

    struct WorkSystem : public System {
        virtual void Execute(SystemCommands& commands) = 0;
    };

    struct SetupSystem : public WorkSystem {};
    struct TickSystem : public WorkSystem {};

    struct EventSystem : public System {
        virtual void OnReceiveEvent(SystemCommands& commands, const Mirror::Any& eventRef) = 0;
    };

    using SystemExecuteFunc = std::function<void(SystemCommands&)>;
    using SystemOnReceiveEventFunc = std::function<void(SystemCommands&, const Mirror::Any&)>;

    struct FuncSetupSystem : public SetupSystem {
        explicit FuncSetupSystem(SystemExecuteFunc inExecuteFunc) : executeFunc(std::move(inExecuteFunc)) {}

        void Execute(Runtime::SystemCommands &commands) override
        {
            return executeFunc(commands);
        }

        SystemExecuteFunc executeFunc;
    };

    struct FuncTickSystem : public TickSystem {
        explicit FuncTickSystem(SystemExecuteFunc inExecuteFunc) : executeFunc(std::move(inExecuteFunc)) {}

        void Execute(Runtime::SystemCommands &commands) override
        {
            return executeFunc(commands);
        }

        SystemExecuteFunc executeFunc;
    };

    struct FuncEventSystem : public EventSystem {
        explicit FuncEventSystem(SystemOnReceiveEventFunc inOnReceiveEventFunc) : onReceiveEventFunc(std::move(inOnReceiveEventFunc)) {}

        void OnReceiveEvent(Runtime::SystemCommands &commands, const Mirror::Any &eventRef) override
        {
            onReceiveEventFunc(commands, eventRef);
        }

        SystemOnReceiveEventFunc onReceiveEventFunc;
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

    struct ECSHost {
    protected:
        ECSHost() = default;

        virtual void BroadcastSystemEvent(SystemEventSignature eventSignature, const Mirror::Any& eventRef) = 0;

        std::unordered_map<GlobalComponentSignature, Mirror::Any> globalComponents;

    private:
        friend class SystemCommands;

        template <typename E>
        void BroadcastSystemEvent(const E& event)
        {
            Mirror::Any eventRef = std::ref(event);
            BroadcastSystemEvent(Internal::SignForClass<E>(), event);
        }
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

    class RUNTIME_API SystemCommands {
    public:
        NonCopyable(SystemCommands)
        SystemCommands(entt::registry& inRegistry, ECSHost& inHost);
        ~SystemCommands();

        Entity Create();
        void Destroy(Entity inEntity);
        bool Valid(Entity inEntity) const;

        template <typename C, typename... Args>
        void Emplace(Entity entity, Args&&... args)
        {
            registry.emplace<C>(entity, std::forward<Args>(args)...);
            Broadcast(ComponentAdded<C> { {}, entity });
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
            Broadcast(ComponentUpdated<C> { {}, entity });
        }

        template <typename C, typename... Args>
        void Set(Entity entity, Args&&... args)
        {
            registry.replace<C>(entity, std::forward<Args>(args)...);
            Broadcast(ComponentUpdated<C> { {}, entity });
        }

        template <typename C>
        void Updated(Entity entity)
        {
            Broadcast(ComponentUpdated<C> { {}, entity });
        }

        template <typename C>
        void Remove(Entity entity)
        {
            registry.remove<C>(entity);
            Broadcast(ComponentRemoved<C> { {}, entity });
        }

        template <typename G, typename... Args>
        void EmplaceGlobal(Args&&... args)
        {
            GlobalComponentSignature signature = Internal::SignForClass<G>();
            auto iter = host.globalComponents.find(signature);
            Assert(iter == host.globalComponents.end());
            host.globalComponents.emplace(std::make_pair(signature, Mirror::Any(G(std::forward<Args>(args)...))));
            Broadcast(GlobalComponentAdded<G> {});
        }

        template <typename G>
        G* GetGlobal()
        {
            GlobalComponentSignature signature = Internal::SignForClass<G>();
            auto iter = host.globalComponents.find(signature);
            if (iter == host.globalComponents.end()) {
                return nullptr;
            } else {
                return &iter->second.As<G&>();
            }
        }

        template <typename G>
        bool HasGlobal()
        {
            return GetGlobal<G>() != nullptr;
        }

        template <typename G, typename F>
        void PatchGlobal(F&& patchFunc)
        {
            GlobalComponentSignature signature = Internal::SignForClass<G>();
            auto iter = host.globalComponents.find(signature);
            Assert(iter != host.globalComponents.end());
            patchFunc(iter->second.As<G&>());
            Broadcast(GlobalComponentUpdated<G> {});
        }

        template <typename G>
        void UpdatedGlobal()
        {
            Broadcast(GlobalComponentUpdated<G> {});
        }

        template <typename G, typename... Args>
        void SetGlobal(Args&&... args)
        {
            GlobalComponentSignature signature = Internal::SignForClass<G>();
            auto iter = host.globalComponents.find(signature);
            Assert(iter != host.globalComponents.end());
            iter->second = G(std::forward<Args>(args)...);
            Broadcast(GlobalComponentUpdated<G> {});
        }

        template <typename G>
        void RemoveGlobal()
        {
            GlobalComponentSignature signature = Internal::SignForClass<G>();
            auto iter = host.globalComponents.find(signature);
            Assert(iter != host.globalComponents.end());
            host.globalComponents.erase(signature);
            Broadcast(GlobalComponentRemoved<G> {});
        }

        template <typename... C, typename... E>
        Query<entt::exclude_t<E...>, C...> StartQuery(Exclude<E...> = {})
        {
            return Query<entt::exclude_t<E...>, C...>(registry.view<C...>(entt::exclude_t<E...> {}));
        }

        template <typename E>
        void Broadcast(const E& event)
        {
            host.BroadcastSystemEvent<E>(event);
        }

    private:
        ECSHost& host;
        entt::registry& registry;
    };
}
