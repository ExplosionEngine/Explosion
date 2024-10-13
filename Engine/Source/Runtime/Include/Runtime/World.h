//
// Created by johnk on 2024/8/2.
//

#pragma once

#include <taskflow/taskflow.hpp>
#include <entt/entt.hpp>

#include <Mirror/Meta.h>
#include <Mirror/Mirror.h>
#include <Runtime/Api.h>
#include <Common/Serialization.h>

namespace Runtime::Internal {
    template <typename T> const Mirror::Class* GetClassChecked();
    const Mirror::Class* GetClassChecked(const std::string& inName);
}

namespace Runtime {
    using Entity = entt::entity;
    constexpr auto entityNull = entt::null;

    class RUNTIME_API EClass() Component {
        EClassBody(Component)
        Component();
    };

    class RUNTIME_API EClass() State {
        EClassBody(State)
        State();
    };

    class Commands;

    class RUNTIME_API EClass() System {
        EPolyClassBody(System)
        System();
        virtual ~System();

        EFunc() virtual void Setup(Commands& commands) const;
        EFunc() virtual void Tick(Commands& commands, float inTimeMs) const;
    };

    template <typename S> concept StateDerived = std::is_base_of_v<State, S>;
    template <typename C> concept CompDerived = std::is_base_of_v<Component, C>;
    template <typename S> concept SystemDerived = std::is_base_of_v<System, S>;
    template <typename... T> struct Exclude {};
}

namespace Runtime {
    class World;
    class Ticker;

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

    using StateClass = const Mirror::Class*;
    using SystemClass = const Mirror::Class*;

    class RUNTIME_API Commands {
    public:
        ~Commands();

        Entity CreateEntity();
        void DestroyEntity(Entity inEntity);

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
        // TODO runtime view

    private:
        friend class World;

        explicit Commands(World& inWorld);

        World& world;
    };

    // world is fully runtime structure, we use level to perform persist storage
    class RUNTIME_API World {
    public:
        explicit World(std::string inName = "");
        ~World();

        DefaultCopyable(World)
        DefaultMovable(World)

        template <SystemDerived System, typename... Args> void AddSystem(Args&&... inSystemArgs);
        void AddBarrier();

        void Play();
        void Stop();
        void Pause();
        void Resume();
        void Tick(float inFrameTimeMs);
        bool Started() const;
        bool Playing() const;

    private:
        friend class Commands;

        using SystemOp = std::function<void(const System&)>;
        void ExecuteSystemGraph(const SystemOp& inOp);

        bool setuped;
        bool playing;
        std::string name;
        std::unordered_map<StateClass, Mirror::Any> states;
        std::unordered_map<SystemClass, Mirror::Any> systems;
        std::vector<std::vector<SystemClass>> systemsGraph;
        std::vector<SystemClass> systemsInBarriers;
        entt::registry registry;
    };
}

namespace Runtime::Internal {
    template <typename T>
    const Mirror::Class* GetClassChecked()
    {
        return &Mirror::Class::Get<T>();
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
        return iter == world.states.end() ? nullptr : &iter->second.template As<S&>();
    }

    template <StateDerived S>
    const S* Commands::FindState() const
    {
        auto* clazz = Internal::GetClassChecked<S>();
        auto iter = world.states.find(clazz);
        return iter == world.states.end() ? nullptr : &iter->second.template As<const S&>();
    }

    template <StateDerived S>
    S& Commands::GetState()
    {
        Assert(HasState<S>());
        return world.states.at(Internal::GetClassChecked<S>()).template As<S&>();
    }

    template <StateDerived S>
    const S& Commands::GetState() const
    {
        Assert(HasState<S>());
        return world.states.at(Internal::GetClassChecked<S>()).template As<const S&>();
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

    template <SystemDerived System, typename ... Args>
    void World::AddSystem(Args&&... inSystemArgs)
    {
        SystemClass clazz = Internal::GetClassChecked<System>();
        systems.emplace(clazz, System(std::forward<Args>(inSystemArgs)...));
        systemsInBarriers.emplace_back(clazz);
    }
};
