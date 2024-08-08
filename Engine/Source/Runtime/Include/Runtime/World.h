//
// Created by johnk on 2024/8/2.
//

#pragma once

#include <taskflow/taskflow.hpp>

#include <Common/Serialization.h>
#include <Runtime/ECS.h>

namespace Runtime::Internal {
    template <typename T> const Mirror::Class* GetClassChecked(const std::string& inName);
}

namespace Runtime {
    struct RUNTIME_API EClass() WorldStart {
        EClassBody(WorldStart)
    };

    struct RUNTIME_API EClass() WorldTick {
        EClassBody(WorldTick)

        float frameTimeMs;
    };

    class World;

    template <typename... Args>
    class CompView {
    public:
        using Iterator = entt::basic_view<Entity, Args...>;

        explicit CompView(const entt::basic_view<Entity, Args...>& inView);

        template <typename F> void Each(F&& inFunc);

        Iterator Begin() const;
        Iterator End() const;
        Iterator ReverseBegin() const;
        Iterator ReverseEnd() const;
        Iterator begin() const;
        Iterator end() const;

    private:
        entt::basic_view<Entity, Args...> view;
    };

    // TODO runtime view

    class RUNTIME_API Commands {
    public:
        explicit Commands(World& inWorld);
        ~Commands();

        template <StateDerived S> bool HasState() const;
        template <StateDerived S, typename... Args> S& EmplaceState(Args&&... inArgs);
        template <StateDerived S> S* FindState();
        template <StateDerived S> const S* FindState() const;
        template <StateDerived S> S& GetState();
        template <StateDerived S> const S& GetState() const;
        template <CompDerived C> bool HasComp(Entity inEntity) const;
        template <CompDerived C, typename... Args> C& EmplaceComp(Entity inEntity, Args&&... inArgs);
        template <CompDerived C> C* FindComp(Entity inEntity);
        template <CompDerived C> const C* FindComp(Entity inEntity) const;
        template <CompDerived C> C& GetComp(Entity inEntity);
        template <CompDerived C> const C& GetComp(Entity inEntity) const;
        template <typename... C, typename... E> auto View(Exclude<E...> = {});
        template <typename... C, typename... E> auto View(Exclude<E...> = {}) const;
        Entity CreateEntity();
        void DestroyEntity(Entity inEntity);
        // TODO runtime view

    private:
        World& world;
    };

    class RUNTIME_API World {
    public:
        NonCopyable(World)
        explicit World(std::string inName = "");
        ~World();

        template <CompDerived System, typename... Args> void AddSystem(Args&&... inSystemArgs);
        template <EventDerived E, typename... Args> void BroadcastEvent(Args&&... inEventArgs);
        void Duplicate(World& inWorld) const;

    private:
        friend class Common::Serializer<World>;
        friend class Commands;

        using StateClass = const Mirror::Class*;
        using SystemClass = const Mirror::Class*;
        using EventClass = const Mirror::Class*;

        template <EventDerived ListenEvent, EventDerived SignalEvent, CompDerived System, typename... Args>
        void AddSystemInternal(Args&&... inSystemArgs);

        std::string name;
        std::unordered_map<StateClass, Mirror::Any> states;
        std::unordered_map<SystemClass, Mirror::Any> systemObjs;
        std::unordered_map<EventClass, std::vector<SystemClass>> listenersMap;
        std::unordered_map<SystemClass, EventClass> signals;
        entt::registry registry;
    };
}

namespace Runtime::Internal {
    template <typename T>
    const Mirror::Class* GetClassChecked(const std::string& inName)
    {
        return &Mirror::Class::Get(inName);
    }
}

namespace Runtime {
    template <typename ... Args>
    CompView<Args...>::CompView(const entt::basic_view<Entity, Args...>& inView)
        : view(inView)
    {
    }

    template <typename ... Args>
    template <typename F>
    void CompView<Args...>::Each(F&& inFunc)
    {
        view.each(std::forward<F>(inFunc));
    }

    template <typename ... Args>
    typename CompView<Args...>::Iterator CompView<Args...>::Begin() const
    {
        return view.begin();
    }

    template <typename ... Args>
    typename CompView<Args...>::Iterator CompView<Args...>::End() const
    {
        return view.end();
    }

    template <typename ... Args>
    typename CompView<Args...>::Iterator CompView<Args...>::ReverseBegin() const
    {
        return view.rbegin();
    }

    template <typename ... Args>
    typename CompView<Args...>::Iterator CompView<Args...>::ReverseEnd() const
    {
        return view.rend();
    }

    template <typename ... Args>
    typename CompView<Args...>::Iterator CompView<Args...>::begin() const
    {
        return Begin();
    }

    template <typename ... Args>
    typename CompView<Args...>::Iterator CompView<Args...>::end() const
    {
        return End();
    }

    template <StateDerived S>
    bool Commands::HasState() const
    {
        return world.states.contains(Internal::GetClassChecked<S>());
    }

    template <StateDerived S, typename ... Args>
    S& Commands::EmplaceState(Args&&... inArgs)
    {
        world.states.emplace(Internal::GetClassChecked<S>(), Mirror::Any(S(std::forward<S>(inArgs)...)));
        return GetState<S>();
    }

    template <StateDerived S>
    S* Commands::FindState()
    {
        auto* clazz = Internal::GetClassChecked<S>();
        auto iter = world.states.find(clazz);
        return iter == world.states.end() ? nullptr : &iter->second.As<S&>();
    }

    template <StateDerived S>
    const S* Commands::FindState() const
    {
        auto* clazz = Internal::GetClassChecked<S>();
        auto iter = world.states.find(clazz);
        return iter == world.states.end() ? nullptr : &iter->second.As<const S&>();
    }

    template <StateDerived S>
    S& Commands::GetState()
    {
        Assert(HasState<S>());
        return world.states.at(Internal::GetClassChecked<S>()).As<S&>();
    }

    template <StateDerived S>
    const S& Commands::GetState() const
    {
        Assert(HasState<S>());
        return world.states.at(Internal::GetClassChecked<S>()).As<const S&>();
    }

    template <CompDerived C>
    bool Commands::HasComp(Entity inEntity) const
    {
        return world.registry.try_get<C>(inEntity) != nullptr;
    }

    template <CompDerived C, typename ... Args>
    C& Commands::EmplaceComp(Entity inEntity, Args&&... inArgs)
    {
        return world.registry.emplace<C>(inEntity, std::forward<Args>(inArgs)...);
    }

    template <CompDerived C>
    C* Commands::FindComp(Entity inEntity)
    {
        return world.registry.try_get<C>(inEntity);
    }

    template <CompDerived C>
    const C* Commands::FindComp(Entity inEntity) const
    {
        return world.registry.try_get<C>(inEntity);
    }

    template <CompDerived C>
    C& Commands::GetComp(Entity inEntity)
    {
        auto* result = world.registry.try_get<C>(inEntity);
        Assert(result != nullptr);
        return *result;
    }

    template <CompDerived C>
    const C& Commands::GetComp(Entity inEntity) const
    {
        auto* result = world.registry.try_get<C>(inEntity);
        Assert(result != nullptr);
        return *result;
    }

    template <typename ... C, typename ... E>
    auto Commands::View(Exclude<E...>)
    {
        return CompView<entt::exclude_t<E...>, C...>(world.registry.view<C...>(entt::exclude_t<E...> {}));
    }

    template <typename ... C, typename ... E>
    auto Commands::View(Exclude<E...>) const
    {
        return CompView<entt::exclude_t<E...>, std::add_const_t<C>...>(world.registry.view<C...>(entt::exclude_t<E...> {}));
    }

    template <CompDerived System, typename ... Args>
    void World::AddSystem(Args&&... inSystemArgs)
    {
        using ListenEvent = typename SystemFuncTraits<decltype(&System::Execute)>::ListenEvent;
        using SignalEvent = typename SystemFuncTraits<decltype(&System::Execute)>::SignalEvent;

        AddSystemInternal<ListenEvent, SignalEvent, System>(std::forward<Args>(inSystemArgs)...);
    }

    template <EventDerived E, typename ... Args>
    void World::BroadcastEvent(Args&&... inEventArgs)
    {
        // TODO
    }

    template <EventDerived ListenEvent, EventDerived SignalEvent, CompDerived System, typename... Args>
    void World::AddSystemInternal(Args&&... inSystemArgs)
    {
        static_assert(!std::is_same_v<ListenEvent, void>);

        const auto* systemClass = Internal::GetClassChecked<System>();
        const auto* listenEventClass = Internal::GetClassChecked<ListenEvent>();
        const Mirror::Class* signalEventClass = nullptr;
        if constexpr (!std::is_same_v<SignalEvent, void>) {
            signalEventClass = Internal::GetClassChecked<SignalEvent>();
        }

        Assert(!systemObjs.contains(systemClass) && !signals.contains(systemClass));
        systemObjs.emplace(systemClass, Mirror::Any(System(std::forward<Args>(inSystemArgs)...)));
        listenersMap[listenEventClass].emplace_back(systemClass);
        if (signalEventClass != nullptr) {
            signals.emplace(systemClass, signalEventClass);
        }
    }
};
