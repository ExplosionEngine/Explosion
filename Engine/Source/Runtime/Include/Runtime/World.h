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

    class Commands;

    class RUNTIME_API EClass() System {
        EPolyClassBody(System)
        System();
        virtual ~System();

        virtual void Setup(Commands& inCommands) const;
        virtual void Tick(Commands& inCommands, float inTimeMs) const;
        virtual void Stop(Commands& inCommands) const;
    };

    template <typename S> concept SystemDerived = std::is_base_of_v<System, S>;
    template <typename... T> struct Exclude {};
}

namespace Runtime {
    class World;
    class Ticker;

    template <typename... Args> class BasicCollector;
    using Collector = BasicCollector<>;

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
    using CompClass = const Mirror::Class*;
    using SystemClass = const Mirror::Class*;

    class Commands;

    class RUNTIME_API Observer {
    public:
        using Iterator = entt::observer::iterator;

        template <typename... Matchers> Observer(Commands& inCommands, const BasicCollector<Matchers...>& inCollector);

        template <typename F> void Each(F&& inFunc);
        void Clear();
        Iterator Begin() const;
        Iterator End() const;
        Iterator begin() const;
        Iterator end() const;

    private:
        entt::observer observer;
    };

    struct EntityCompRegistry {
        entt::registry native;
    };

    class RUNTIME_API Commands {
    public:
        ~Commands();

        Entity CreateEntity();
        void DestroyEntity(Entity inEntity);
        bool HasEntity(Entity inEntity) const;

        template <typename S> bool HasState() const;
        template <typename S, typename... Args> const S& EmplaceState(Args&&... inArgs);
        template <typename S> const S* FindState() const;
        template <typename S> const S& GetState() const;
        template <typename S> void RemoveState();
        template <typename S, typename F> void PatchState(F&& inFunc);
        template <typename C> bool HasComp(Entity inEntity) const;
        template <typename C, typename... Args> const C& EmplaceComp(Entity inEntity, Args&&... inArgs);
        template <typename C> const C* FindComp(Entity inEntity) const;
        template <typename C> const C& GetComp(Entity inEntity) const;
        template <typename C> void RemoveComp(Entity inEntity);
        template <typename C, typename F> void PatchComp(Entity inEntity, F&& inFunc);
        template <typename... C, typename... E> auto View(Exclude<E...> = {});
        template <typename... C, typename... E> auto View(Exclude<E...> = {}) const;
        // TODO runtime view
        template <typename... M> Observer CreateObserver(const BasicCollector<M...>& inCollector);
        // TODO component lifecycle listeners

    private:
        friend class World;
        friend class Observer;

        explicit Commands(EntityCompRegistry& inRegistry);

        EntityCompRegistry& registry;
    };

    struct SystemRegistry {
        std::unordered_map<SystemClass, Mirror::Any> systems;
        std::vector<std::vector<SystemClass>> systemsGraph;
        std::vector<SystemClass> systemsInBarriers;
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
        friend class Observer;

        using SystemOp = std::function<void(const System&)>;

        // TODO we not want read-write in a barrier time, so we maybe need runtime read-write check
        void ExecuteSystemGraph(const SystemOp& inOp);

        bool setup;
        bool playing;
        std::string name;
        EntityCompRegistry entityCompRegistry;
        SystemRegistry systemRegistry;
    };
}

namespace Runtime::Internal {
    template <typename T>
    const Mirror::Class* GetClassChecked()
    {
        return &Mirror::Class::Get<T>();
    }

    template <typename LibType>
    struct CollectorConverter {};

    template <>
    struct CollectorConverter<entt::basic_collector<>> {
        using EngineType = BasicCollector<>;
    };

    template <typename... Reject, typename... Require, typename... Rule, typename... Other>
    struct CollectorConverter<entt::basic_collector<entt::matcher<entt::type_list<Reject...>, entt::type_list<Require...>, Rule...>, Other...>> {
        using EngineType = BasicCollector<entt::matcher<entt::type_list<Reject...>, entt::type_list<Require...>, Rule...>, Other...>;
    };
}

namespace Runtime {
    template <>
    class BasicCollector<> {
    public:
        BasicCollector() = default;

        template <typename... AllOf, typename... NoneOf>
        static constexpr auto Group(Exclude<NoneOf...> = {}) noexcept
        {
            using LibRetType = decltype(entt::basic_collector<>::group<AllOf..., NoneOf...>(entt::exclude<NoneOf...>));
            return typename Internal::CollectorConverter<LibRetType>::EngineType {};
        }

        template <typename AnyOf>
        static constexpr auto Update() noexcept
        {
            using LibRetType = decltype(entt::basic_collector<>::update<AnyOf>());
            return typename Internal::CollectorConverter<LibRetType>::EngineType {};
        }

    private:
        friend class Observer;

        static auto Final() noexcept
        {
            return entt::basic_collector<> {};
        }
    };

    template <typename... Args>
    class BasicCollector {
    public:
        BasicCollector() = default;

        template<typename... AllOf, typename... NoneOf>
        static constexpr auto Group(Exclude<NoneOf...> = {}) noexcept
        {
            using LibRetType = decltype(entt::basic_collector<Args...>::template group<AllOf..., NoneOf...>(entt::exclude<NoneOf...>));
            return typename Internal::CollectorConverter<LibRetType>::EngineType {};
        }

        template<typename AnyOf>
        static constexpr auto Update() noexcept
        {
            using LibRetType = decltype(entt::basic_collector<Args...>::template update<AnyOf>());
            return typename Internal::CollectorConverter<LibRetType>::EngineType {};
        }

        template<typename... AllOf, typename... NoneOf>
        static constexpr auto Where(Exclude<NoneOf...> = {}) noexcept
        {
            using LibRetType = decltype(entt::basic_collector<Args...>::template where<AllOf..., NoneOf...>(entt::exclude<NoneOf...>));
            return typename Internal::CollectorConverter<LibRetType>::EngineType {};
        }

    private:
        friend class Observer;

        static auto Final() noexcept
        {
            return entt::basic_collector<Args...> {};
        }
    };

    template <typename ... Matchers>
    Observer::Observer(Commands& inCommands, const BasicCollector<Matchers...>& inCollector)
        : observer(inCommands.registry, inCollector.Final())
    {
    }

    template <typename F>
    void Observer::Each(F&& inFunc)
    {
        observer.each(std::forward<F>(inFunc));
    }

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

    template <typename S>
    bool Commands::HasState() const
    {
        return registry.native.try_ctx<S>() != nullptr;
    }

    template <typename S, typename ... Args>
    const S& Commands::EmplaceState(Args&&... inArgs)
    {
        return registry.native.set<S>(std::forward<Args>(inArgs)...);
    }

    template <typename S>
    const S* Commands::FindState() const
    {
        return registry.native.try_ctx<S>();
    }

    template <typename S>
    const S& Commands::GetState() const
    {
        return registry.native.ctx<S>();
    }

    template <typename S>
    void Commands::RemoveState() // NOLINT
    {
        registry.native.unset<S>();
    }

    template <typename S, typename F>
    void Commands::PatchState(F&& inFunc)
    {
        Assert(HasState<S>());
        inFunc(registry.native.ctx<S>());
    }

    template <typename C>
    bool Commands::HasComp(Entity inEntity) const
    {
        return registry.native.try_get<C>(inEntity) != nullptr;
    }

    template <typename C, typename ... Args>
    const C& Commands::EmplaceComp(Entity inEntity, Args&&... inArgs)
    {
        return registry.native.emplace<C>(inEntity, std::forward<Args>(inArgs)...);
    }

    template <typename C>
    const C* Commands::FindComp(Entity inEntity) const
    {
        return registry.native.try_get<C>(inEntity);
    }

    template <typename C>
    const C& Commands::GetComp(Entity inEntity) const
    {
        auto* result = registry.native.try_get<C>(inEntity);
        Assert(result != nullptr);
        return *result;
    }

    template <typename C>
    void Commands::RemoveComp(Entity inEntity) // NOLINT
    {
        registry.native.erase<C>(inEntity);
    }

    template <typename C, typename F>
    void Commands::PatchComp(Entity inEntity, F&& inFunc)
    {
        registry.native.patch<C>(inEntity, std::forward<F>(inFunc));
    }

    template <typename ... C, typename ... E>
    auto Commands::View(Exclude<E...>)
    {
        return CompView<entt::exclude_t<E...>, C...>(registry.native.view<C...>(entt::exclude_t<E...> {}));
    }

    template <typename ... C, typename ... E>
    auto Commands::View(Exclude<E...>) const
    {
        return CompView<entt::exclude_t<E...>, std::add_const_t<C>...>(registry.native.view<C...>(entt::exclude_t<E...> {}));
    }

    template <typename ... M>
    Observer Commands::CreateObserver(const BasicCollector<M...>& inCollector)
    {
        return { *this, inCollector };
    }

    template <SystemDerived System, typename ... Args>
    void World::AddSystem(Args&&... inSystemArgs)
    {
        SystemClass clazz = Internal::GetClassChecked<System>();
        systemRegistry.systems.emplace(clazz, System(std::forward<Args>(inSystemArgs)...));
        systemRegistry.systemsInBarriers.emplace_back(clazz);
    }
};
