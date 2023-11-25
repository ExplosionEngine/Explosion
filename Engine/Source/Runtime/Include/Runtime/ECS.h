//
// Created by johnk on 2022/8/10.
//

#pragma once

#include <tuple>

#include <entt/entt.hpp>
#include <taskflow/taskflow.hpp>

#include <Common/Utility.h>
#include <Common/Hash.h>
#include <Common/String.h>
#include <Common/Memory.h>
#include <Mirror/Meta.h>
#include <Mirror/Mirror.h>
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

#define DeclareCompTypeGetter(compClass) \
    EFunc() \
    static const Runtime::ComponentType& GetCompType() \
    { \
        return Runtime::Internal::compTypeImpl<compClass>; \
    } \

#define EComponentBody(compClass) \
    EClassBody(compClass) \
    DeclareCompLifecycleEvents() \
    DeclareCompTypeGetter(compClass) \

#define DeclareSingleStateLifecycleEvent(eventClass) \
    struct EClass() eventClass : public Runtime::Event { \
        EClassBody(eventClass) \
    }; \

#define DeclareStateLifecycleEvents() \
    DeclareSingleStateLifecycleEvent(Added) \
    DeclareSingleStateLifecycleEvent(Updated) \
    DeclareSingleStateLifecycleEvent(Removed) \

#define DeclareStateTypeGetter(stateClass) \
    EFunc() \
    static const Runtime::StateType& GetStateType() \
    { \
        return Runtime::Internal::stateTypeImpl<stateClass>; \
    } \

#define EStateBody(stateClass) \
    EClassBody(stateClass) \
    DeclareStateLifecycleEvents() \
    DeclareStateTypeGetter(stateClass) \

#define DeclareSystemDependencies(...) \
    using Dependencies = std::tuple<__VA_ARGS__>; \

#define DeclareSetupSystemTypeGetter(systemClass) \
    EFunc() \
    static const Runtime::SystemType& GetSystemType() \
    { \
        return Runtime::Internal::setupSystemTypeImpl<systemClass>; \
    } \

#define ESetupSystemBody(systemClass) \
    EClassBody(systemClass) \
    DeclareSetupSystemTypeGetter(systemClass) \

#define DeclareTickSystemTypeGetter(systemClass) \
    EFunc() \
    static const Runtime::SystemType& GetSystemType() \
    { \
        return Runtime::Internal::tickSystemTypeImpl<systemClass>; \
    } \

#define ETickSystemBody(systemClass) \
    EClassBody(systemClass) \
    DeclareTickSystemTypeGetter(systemClass) \

#define DeclareEventSystemTypeGetter(systemClass, eventClass) \
    EFunc() \
    static const Runtime::SystemType& GetSystemType()         \
    { \
        return Runtime::Internal::eventSystemTypeImpl<eventClass, systemClass>; \
    } \

#define EEventSystemBody(systemClass, eventClass) \
    EClassBody(systemClass) \
    DeclareEventSystemTypeGetter(systemClass, eventClass) \

namespace Runtime {
    enum class ClassSignatureType {
        staticClass,
        max
    };

    struct RUNTIME_API ClassSignature {
        ClassSignatureType type;
        size_t id;
        std::string name;

        bool operator==(const ClassSignature& rhs) const;
    };

    using ComponentSignature = ClassSignature;
    using StateSignature = ClassSignature;
    using SystemSignature = ClassSignature;
    using EventSignature = ClassSignature;
}

template <>
struct std::hash<Runtime::ClassSignature> {
    size_t operator()(const Runtime::ClassSignature& value) const noexcept
    {
        return std::hash<size_t>{}(value.id);
    }
};

namespace Runtime::Internal {
    template <typename C>
    ClassSignature SignForStaticClass();

    template <typename S, typename C = void>
    struct SystemHasDependencies : std::false_type {};

    template <typename S>
    struct SystemHasDependencies<S, std::void_t<typename S::Dependencies>> : std::true_type {};

    template <typename Dependencies, size_t... I>
    std::vector<SystemSignature> BuildDependencyListForStaticSystemInternal(std::index_sequence<I...>);

    template <typename S>
    std::vector<SystemSignature> BuildDependencyListForStaticSystem();
}

namespace Runtime {
    class SystemCommands;
    struct ECSHost;

    using Entity = entt::entity;
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

    template <typename... S>
    struct SetupSystemPackage {
        using SystemTuple = std::tuple<S...>;
    };

    template <typename... S>
    struct TickSystemPackage {
        using SystemTuple = std::tuple<S...>;
    };

    template <typename E, typename... S>
    struct EventSystemPackage {
        using Event = E;
        using SystemTuple = std::tuple<S...>;
    };

    template <typename SP = SetupSystemPackage<>, typename TP = TickSystemPackage<>, typename... ESP>
    struct SystemPackage {
        using Setup = SP;
        using Tick = TP;
        using EventSystemPackageTuple = std::tuple<ESP...>;
    };

    enum class SystemRole : uint8_t {
        setup,
        tick,
        event,
        max
    };

    struct ComponentType;
    struct StateType;

    struct RUNTIME_API ECSHost {
    public:
        template <typename S>
        void AddSetupSystem();

        template <typename S>
        void AddTickSystem();

        template <typename E, typename S>
        void AddEventSystem();

        template <typename S>
        void RemoveSetupSystem();

        template <typename S>
        void RemoveTickSystem();

        template <typename E, typename S>
        void RemoveEventSystem();

        template <typename E>
        void BroadcastEvent(const E& event);

        template <typename P>
        void AddSystemPackage();

        template <typename P>
        void RemoveSystemPackage();

    protected:
        virtual void Reset();
        virtual void Setup();
        virtual void Tick(float timeMS);
        virtual void Shutdown();
        virtual bool Setuped();

        ECSHost() = default;

        using SetupProxyFunc = std::function<void(SystemCommands&)>;
        using TickProxyFunc = std::function<void(SystemCommands&, float timeMS)>;
        using OnReceiveProxyFunc = std::function<void(SystemCommands&, Mirror::Any*)>;

        struct RUNTIME_API SystemInstance {
            SystemRole type;
            Common::UniqueRef<System> object;
            std::variant<SetupProxyFunc, TickProxyFunc , OnReceiveProxyFunc> proxy;

            SystemInstance();
            ~SystemInstance();
            SystemInstance(SystemInstance&& other) noexcept;
        };

        bool setuped;
        entt::registry registry;
        std::unordered_map<ComponentSignature, const ComponentType*> componentTypes;
        std::unordered_map<ComponentSignature, const StateType*> stateTypes;
        std::unordered_map<SystemSignature, SystemInstance> systemInstances;
        std::unordered_set<SystemSignature> setupSystems;
        std::unordered_set<SystemSignature> tickSystems;
        std::unordered_map<EventSignature, std::unordered_set<SystemSignature>> eventSystems;
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

        explicit Query(entt::view<Args...>&& inView);
        ~Query();

        template <typename F>
        void Each(F&& func);

        auto Each();

    private:
        entt::view<Args...> view;
    };

    template <typename... C>
    struct Exclude {};

    class RUNTIME_API SystemCommands {
    public:
        NonCopyable(SystemCommands)
        explicit SystemCommands(ECSHost& inHost);
        ~SystemCommands();

        Entity Create(Entity hint = entityNull);
        void Destroy(Entity inEntity);
        bool Valid(Entity inEntity) const;

        template <typename F>
        void Each(F&& func);

        template <typename C, typename... Args>
        void Emplace(Entity entity, Args&&... args);

        template <typename C>
        C* Get(Entity entity);

        template <typename C>
        bool Has(Entity entity);

        template <typename C, typename F>
        void Patch(Entity entity, F&& patchFunc);

        template <typename C, typename... Args>
        void Set(Entity entity, Args&&... args);

        template <typename C>
        void Updated(Entity entity);

        template <typename C>
        void Remove(Entity entity);

        template <typename S, typename... Args>
        void EmplaceState(Args&&... args);

        template <typename S>
        S* GetState();

        template <typename S>
        bool HasState();

        template <typename S, typename F>
        void PatchState(F&& patchFunc);

        template <typename S, typename... Args>
        void SetState(Args&&... args);

        template <typename S>
        void UpdatedState();

        template <typename S>
        void RemoveState();

        template <typename... C, typename... E>
        Query<entt::exclude_t<E...>, C...> StartQuery(Exclude<E...> = {});

        template <typename E>
        void Broadcast(const E& event);

    private:
        ECSHost& host;
        entt::registry& registry;
    };

    struct ComponentType {
        using EmplaceProxy = void(SystemCommands&, Entity, const Mirror::Any&);
        using GetProxy = Mirror::Any(SystemCommands&, Entity);
        using HasProxy = bool(SystemCommands&, Entity);
        using PatchProxy = void(SystemCommands&, Entity, std::function<void(const Mirror::Any&)>);
        using SetProxy = void(SystemCommands&, Entity, const Mirror::Any&);
        using UpdatedProxy = void(SystemCommands&, Entity);
        using RemoveProxy = void(SystemCommands&, Entity);

        template <typename C>
        static void EmplaceImpl(SystemCommands& commands, Entity entity, const Mirror::Any& ref);

        template <typename C>
        static Mirror::Any GetImpl(SystemCommands& commands, Entity entity);

        template <typename C>
        static bool HasImpl(SystemCommands& commands, Entity entity);

        template <typename C>
        static void PatchImpl(SystemCommands& commands, Entity entity, std::function<void(const Mirror::Any&)> patchFunc);

        template <typename C>
        static void SetImpl(SystemCommands& commands, Entity entity, const Mirror::Any& ref);

        template <typename C>
        static void UpdatedImpl(SystemCommands& commands, Entity entity);

        template <typename C>
        static void RemoveImpl(SystemCommands& commands, Entity entity);

        EmplaceProxy* emplace;
        GetProxy* get;
        HasProxy* has;
        PatchProxy* patch;
        SetProxy* set;
        UpdatedProxy* updated;
        RemoveProxy* remove;
    };

    struct StateType {
        using EmplaceProxy = void(SystemCommands&, const Mirror::Any&);
        using GetProxy = Mirror::Any(SystemCommands&);
        using HasProxy = bool(SystemCommands&);
        using PatchProxy = void(SystemCommands&, std::function<void(const Mirror::Any&)>);
        using SetProxy = void(SystemCommands&, const Mirror::Any&);
        using UpdatedProxy = void(SystemCommands&);
        using RemoveProxy = void(SystemCommands&);

        template <typename S>
        static void EmplaceImpl(SystemCommands& commands, const Mirror::Any& ref);

        template <typename S>
        static Mirror::Any GetImpl(SystemCommands& commands);

        template <typename S>
        static bool HasImpl(SystemCommands& commands);

        template <typename S>
        static void PatchImpl(SystemCommands& commands, std::function<void(const Mirror::Any&)> patchFunc);

        template <typename S>
        static void SetImpl(SystemCommands& commands, const Mirror::Any& ref);

        template <typename S>
        static void UpdatedImpl(SystemCommands& commands);

        template <typename S>
        static void RemoveImpl(SystemCommands& commands);

        EmplaceProxy* emplace;
        GetProxy* get;
        HasProxy* has;
        PatchProxy* patch;
        SetProxy* set;
        UpdatedProxy* updated;
        RemoveProxy* remove;
    };

    struct SystemType {
        using AddSystemProxy = void(ECSHost&);
        using RemoveSystemProxy = void(ECSHost&);

        template <typename S>
        static void AddSetupSystemImpl(ECSHost& host);

        template <typename S>
        static void RemoveSetupSystemImpl(ECSHost& host);

        template <typename S>
        static void AddTickSystemImpl(ECSHost& host);

        template <typename S>
        static void RemoveTickSystemImpl(ECSHost& host);

        template <typename E, typename S>
        static void AddEventSystemImpl(ECSHost& host);

        template <typename E, typename S>
        static void RemoveEventSystemImpl(ECSHost& host);

        AddSystemProxy* add;
        RemoveSystemProxy* remove;
    };

    struct RUNTIME_API CompTypeFinder {
        static const ComponentType* FromCompClass(const Mirror::Class& compClass);
        static const ComponentType* FromCompClassName(const std::string& compClassName);
    };

    struct RUNTIME_API StateTypeFinder {
        static const StateType* FromStateClass(const Mirror::Class& stateClass);
        static const StateType* FromStateClassName(const std::string& stateClassName);
    };

    struct RUNTIME_API SystemTypeFinder {
        static const SystemType* FromSystemClass(const Mirror::Class& systemClass);
        static const SystemType* FromSystemClassName(const std::string& systemClassName);
    };
}

namespace Runtime::Internal {
    template <typename C>
    ClassSignature SignForStaticClass()
    {
        static ClassSignature signature = []() -> ClassSignature {
            const Mirror::Class& clazz = C::GetClass();

            std::tuple<ClassSignatureType, Mirror::TypeId> source = { ClassSignatureType::staticClass, clazz.GetTypeInfo()->id };

            ClassSignature result;
            result.type = ClassSignatureType::staticClass;
            result.id = clazz.GetTypeInfo()->id;
            result.name = clazz.GetName();
            return result;
        }();
        return signature;
    }

    template <typename Dependencies, size_t... I>
    std::vector<SystemSignature> BuildDependencyListForStaticSystemInternal(std::index_sequence<I...>)
    {
        std::vector<SystemSignature> result(std::tuple_size_v<Dependencies>);
        (void) std::initializer_list<int> { ([&]() -> void {
            result[I] = Internal::SignForStaticClass<std::tuple_element_t<I, Dependencies>>();
        }(), 0)... };
        return result;
    }

    template <typename S>
    std::vector<SystemSignature> BuildDependencyListForStaticSystem()
    {
        if constexpr (SystemHasDependencies<S>::value) {
            using Dependencies = typename S::Dependencies;
            return BuildDependencyListForStaticSystemInternal<Dependencies>(std::make_index_sequence<std::tuple_size_v<Dependencies>> {});
        } else {
            return std::vector<SystemSignature> {};
        }
    }

    template <typename SystemTuple, size_t... I>
    void AddSetupSystemPackage(ECSHost& host, std::index_sequence<I...>)
    {
        std::initializer_list<int> { (0, [&]() -> void { host.AddSetupSystem<std::tuple_element_t<I, SystemTuple>>(); })... };
    }

    template <typename SystemTuple, size_t... I>
    void AddTickSystemPackage(ECSHost& host, std::index_sequence<I...>)
    {
        std::initializer_list<int> { (0, [&]() -> void { host.AddTickSystem<std::tuple_element_t<I, SystemTuple>>(); })... };
    }

    template <typename Event, typename SystemTuple, size_t... I>
    void AddEventSystemPackage(ECSHost& host, std::index_sequence<I...>)
    {
        std::initializer_list<int> { (0, [&]() -> void { host.AddEventSystem<Event, std::tuple_element_t<I, SystemTuple>>(); })... };
    }

    template <typename ESPTuple, size_t... I>
    void AddEventSystemPackageTuple(ECSHost& host, std::index_sequence<I...>)
    {
        std::initializer_list<int> { (0, [&]() -> void {
            using ESP = std::tuple_element_t<I, ESPTuple>;
            using Event = typename ESP::Event;
            using SystemTuple = typename ESP::SystemTuple;
            AddEventSystemPackage<Event, SystemTuple>(std::make_index_sequence<std::tuple_size_v<SystemTuple>> {});
        })... };
    }

    template <typename SystemTuple, size_t... I>
    void RemoveSetupSystemPackage(ECSHost& host, std::index_sequence<I...>)
    {
        std::initializer_list<int> { (0, [&]() -> void { host.RemoveSetupSystem<std::tuple_element_t<I, SystemTuple>>(); })... };
    }

    template <typename SystemTuple, size_t... I>
    void RemoveTickSystemPackage(ECSHost& host, std::index_sequence<I...>)
    {
        std::initializer_list<int> { (0, [&]() -> void { host.RemoveTickSystem<std::tuple_element_t<I, SystemTuple>>(); })... };
    }

    template <typename Event, typename SystemTuple, size_t... I>
    void RemoveEventSystemPackage(ECSHost& host, std::index_sequence<I...>)
    {
        std::initializer_list<int> { (0, [&]() -> void { host.RemoveEventSystem<Event, std::tuple_element_t<I, SystemTuple>>(); })... };
    }

    template <typename ESPTuple, size_t... I>
    void RemoveEventSystemPackageTuple(ECSHost& host, std::index_sequence<I...>)
    {
        std::initializer_list<int> { (0, [&]() -> void {
            using ESP = std::tuple_element_t<I, ESPTuple>;
            using Event = typename ESP::Event;
            using SystemTuple = typename ESP::SystemTuple;
            RemoveEventSystemPackage<Event, SystemTuple>(std::make_index_sequence<std::tuple_size_v<SystemTuple>> {});
        })... };
    }

    template <typename C>
    inline constexpr ComponentType compTypeImpl = {
        &ComponentType::EmplaceImpl<C>,
        &ComponentType::GetImpl<C>,
        &ComponentType::HasImpl<C>,
        &ComponentType::PatchImpl<C>,
        &ComponentType::SetImpl<C>,
        &ComponentType::UpdatedImpl<C>,
        &ComponentType::RemoveImpl<C>
    };

    template <typename S>
    inline constexpr StateType stateTypeImpl = {
        &StateType::EmplaceImpl<S>,
        &StateType::GetImpl<S>,
        &StateType::HasImpl<S>,
        &StateType::PatchImpl<S>,
        &StateType::SetImpl<S>,
        &StateType::UpdatedImpl<S>,
        &StateType::RemoveImpl<S>
    };

    template <typename S>
    inline constexpr SystemType setupSystemTypeImpl = {
        &SystemType::AddSetupSystemImpl<S>,
        &SystemType::RemoveSetupSystemImpl<S>
    };

    template <typename S>
    inline constexpr SystemType tickSystemTypeImpl = {
        &SystemType::AddTickSystemImpl<S>,
        &SystemType::RemoveTickSystemImpl<S>
    };

    template <typename E, typename S>
    inline constexpr SystemType eventSystemTypeImpl = {
        &SystemType::AddEventSystemImpl<E, S>,
        &SystemType::RemoveEventSystemImpl<E, S>
    };
}

namespace Runtime {
    template <typename S>
    void ECSHost::AddSetupSystem()
    {
        SystemSignature signature = Internal::SignForStaticClass<S>();
        Assert(!systemInstances.contains(signature) && !setupSystems.contains(signature) && !setupSystemDependencies.contains(signature));

        auto* object = new S();
        SystemInstance instance;
        instance.type = SystemRole::setup;
        instance.object = object;
        instance.proxy = [object](SystemCommands& commands) -> void {
            object->Setup(commands);
        };

        systemInstances.emplace(std::make_pair(signature, std::move(instance)));
        setupSystems.emplace(signature);
        setupSystemDependencies.emplace(std::make_pair(signature, Internal::BuildDependencyListForStaticSystem<S>()));
    }

    template <typename S>
    void ECSHost::AddTickSystem()
    {
        SystemSignature signature = Internal::SignForStaticClass<S>();
        Assert(!systemInstances.contains(signature) && !tickSystems.contains(signature) && !tickSystemDependencies.contains(signature));

        auto* object = new S();
        SystemInstance instance;
        instance.type = SystemRole::tick;
        instance.object = object;
        instance.proxy = [object](SystemCommands& commands, float timeMS) -> void {
            object->Tick(commands, timeMS);
        };

        systemInstances.emplace(std::make_pair(signature, std::move(instance)));
        tickSystems.emplace(signature);
        tickSystemDependencies.emplace(std::make_pair(signature, Internal::BuildDependencyListForStaticSystem<S>()));
    }

    template <typename E, typename S>
    void ECSHost::AddEventSystem()
    {
        EventSignature eventSignature = Internal::SignForStaticClass<E>();
        if (!eventSystems.contains(eventSignature)) {
            eventSystems.emplace(std::make_pair(eventSignature, std::unordered_set<SystemSignature> {}));
            eventSystemDependencies.emplace(std::make_pair(eventSignature, std::unordered_map<SystemSignature, std::vector<SystemSignature>> {}));
        }

        auto& systems = eventSystems.at(eventSignature);
        auto& systemDependencies = eventSystemDependencies.at(eventSignature);
        SystemSignature systemSignature = Internal::SignForStaticClass<S>();
        Assert(!systemInstances.contains(systemSignature) && !systems.contains(systemSignature) && !systemDependencies.contains(systemSignature));

        auto* object = new S();
        SystemInstance instance;
        instance.type = SystemRole::event;
        instance.object = object;
        instance.proxy = [object](SystemCommands& commands, Mirror::Any* eventRef) -> void {
            object->OnReceive(commands, eventRef->As<const E&>());
        };

        systemInstances.emplace(std::make_pair(systemSignature, std::move(instance)));
        systems.emplace(systemSignature);
        systemDependencies.emplace(std::make_pair(systemSignature, Internal::BuildDependencyListForStaticSystem<S>()));
    }

    template <typename S>
    void ECSHost::RemoveSetupSystem()
    {
        SystemSignature signature = Internal::SignForStaticClass<S>();
        systemInstances.erase(signature);
        setupSystems.erase(signature);
        setupSystemDependencies.erase(signature);
    }

    template <typename S>
    void ECSHost::RemoveTickSystem()
    {
        SystemSignature signature = Internal::SignForStaticClass<S>();
        systemInstances.erase(signature);
        tickSystems.erase(signature);
        tickSystemDependencies.erase(signature);
    }

    template <typename E, typename S>
    void ECSHost::RemoveEventSystem()
    {
        EventSignature eventSignature = Internal::SignForStaticClass<E>();
        Assert(eventSystems.contains(eventSignature) && eventSystemDependencies.contains(eventSignature));

        auto& systems = eventSystems.at(eventSignature);
        auto& systemDependencies = eventSystemDependencies.at(eventSignature);

        SystemSignature systemSignature = Internal::SignForStaticClass<S>();
        systemInstances.erase(systemSignature);
        systems.erase(systemSignature);
        systemDependencies.erase(systemSignature);
    }

    template <typename E>
    void ECSHost::BroadcastEvent(const E& event)
    {
        EventSignature eventSignature = Internal::SignForStaticClass<E>();
        if (!eventSystems.contains(eventSignature)) {
            return;
        }

        const auto& systems = eventSystems.at(eventSignature);
        const auto& systemDependencies = eventSystemDependencies.at(eventSignature);

        tf::Taskflow taskflow;
        std::unordered_map<SystemSignature, tf::Task> tasks;
        tasks.reserve(systems.size());

        SystemCommands systemCommands(*this);
        Mirror::Any eventRef = std::ref(event);

        for (const auto& system : systems) {
            const auto& systemInstance = systemInstances.at(system);
            Assert(systemInstance.type == SystemRole::event);

            tasks.emplace(std::make_pair(system, taskflow.emplace([&]() -> void {
                std::get<OnReceiveProxyFunc>(systemInstance.proxy)(systemCommands, &eventRef);
            })));
        }

        for (const auto& dependencies : systemDependencies) {
            auto& task = tasks.at(dependencies.first);
            for (const auto& depend : dependencies.second) {
                task.succeed(tasks.at(depend));
            }
        }

        tf::Executor executor;
        executor.run(taskflow);
    }

    template <typename P>
    void ECSHost::AddSystemPackage()
    {
        using SetupSystemTuple = typename P::Setup::SystemTuple;
        Internal::AddSetupSystemPackage<SetupSystemTuple>(std::make_index_sequence<std::tuple_size_v<SetupSystemTuple>> {});

        using TickSystemTuple = typename P::Tick::SystemTuple;
        Internal::AddTickSystemPackage<TickSystemTuple>(std::make_index_sequence<std::tuple_size_v<TickSystemTuple>> {});

        using ESPTuple = typename P::EventSystemPackageTuple;
        Internal::AddEventSystemPackageTuple<ESPTuple>(std::make_index_sequence<std::tuple_size_v<ESPTuple>> {});
    }

    template <typename P>
    void ECSHost::RemoveSystemPackage()
    {
        using SetupSystemTuple = typename P::Setup::SystemTuple;
        Internal::RemoveSetupSystemPackage<SetupSystemTuple>(std::make_index_sequence<std::tuple_size_v<SetupSystemTuple>> {});

        using TickSystemTuple = typename P::Tick::SystemTuple;
        Internal::RemoveTickSystemPackage<TickSystemTuple>(std::make_index_sequence<std::tuple_size_v<TickSystemTuple>> {});

        using ESPTuple = typename P::EventSystemPackageTuple;
        Internal::RemoveEventSystemPackageTuple<ESPTuple>(std::make_index_sequence<std::tuple_size_v<ESPTuple>> {});
    }

    template <typename... Args>
    Query<Args...>::Query(entt::view<Args...>&& inView)
        : view(std::move(inView))
    {
    }

    template <typename... Args>
    Query<Args...>::~Query() = default;

    template <typename... Args>
    template <typename F>
    void Query<Args...>::Each(F&& func)
    {
        view.each(std::forward<F>(func));
    }

    template <typename... Args>
    auto Query<Args...>::Each()
    {
        return view.each();
    }

    template <typename F>
    void SystemCommands::Each(F&& func)
    {
        registry.each(std::forward<F>(func));
    }

    template <typename C, typename... Args>
    void SystemCommands::Emplace(Entity entity, Args&&... args)
    {
        if (const ComponentSignature signature = Internal::SignForStaticClass<C>();
            !host.componentTypes.contains(signature)) {
            auto* systemType = CompTypeFinder::FromCompClassName(signature.name);
            Assert(systemType != nullptr);
            host.componentTypes.emplace(std::make_pair(signature, systemType));
        }
        registry.emplace<C>(entity, std::forward<Args>(args)...);
        Broadcast(typename C::Added { {}, entity });
    }

    template <typename C>
    C* SystemCommands::Get(Entity entity)
    {
        return registry.try_get<C>(entity);
    }

    template <typename C>
    bool SystemCommands::Has(Entity entity)
    {
        return Get<C>(entity) != nullptr;
    }

    template <typename C, typename F>
    void SystemCommands::Patch(Entity entity, F&& patchFunc)
    {
        registry.patch<C>(entity, patchFunc);
        Broadcast(typename C::Updated { {}, entity });
    }

    template <typename C, typename... Args>
    void SystemCommands::Set(Entity entity, Args&&... args)
    {
        registry.replace<C>(entity, std::forward<Args>(args)...);
        Broadcast(typename C::Updated { {}, entity });
    }

    template <typename C>
    void SystemCommands::Updated(Runtime::Entity entity)
    {
        Broadcast(typename C::Updated { {}, entity });
    }

    template <typename C>
    void SystemCommands::Remove(Runtime::Entity entity)
    {
        registry.remove<C>(entity);
        Broadcast(typename C::Removed { {}, entity });
    }

    template <typename S, typename ...Args>
    void SystemCommands::EmplaceState(Args&& ...args)
    {
        StateSignature signature = Internal::SignForStaticClass<S>();
        auto iter = host.states.find(signature);
        Assert(iter == host.states.end());
        host.states.emplace(std::make_pair(signature, Mirror::Any(S(std::forward<Args>(args)...))));
        Broadcast(typename S::Added {});
    }

    template <typename S>
    S* SystemCommands::GetState()
    {
        StateSignature signature = Internal::SignForStaticClass<S>();
        auto iter = host.states.find(signature);
        if (iter == host.states.end()) {
            return nullptr;
        } else {
            return &iter->second.As<S&>();
        }
    }

    template <typename S>
    bool SystemCommands::HasState()
    {
        return GetState<S>() != nullptr;
    }

    template <typename S, typename F>
    void SystemCommands::PatchState(F&& patchFunc)
    {
        StateSignature signature = Internal::SignForStaticClass<S>();
        auto iter = host.states.find(signature);
        Assert(iter != host.states.end());
        patchFunc(iter->second.As<S&>());
        Broadcast(typename S::Updated {});
    }

    template <typename S, typename ...Args>
    void SystemCommands::SetState(Args&& ...args)
    {
        StateSignature signature = Internal::SignForStaticClass<S>();
        auto iter = host.states.find(signature);
        Assert(iter != host.states.end());
        iter->second = S(std::forward<Args>(args)...);
        Broadcast(typename S::Updated {});
    }

    template <typename S>
    void SystemCommands::UpdatedState()
    {
        Broadcast(typename S::Updated {});
    }

    template <typename S>
    void SystemCommands::RemoveState()
    {
        StateSignature signature = Internal::SignForStaticClass<S>();
        auto iter = host.states.find(signature);
        Assert(iter != host.states.end());
        host.states.erase(signature);
        Broadcast(typename S::Removed {});
    }

    template <typename... C, typename... E>
    Query<entt::exclude_t<E...>, C...> SystemCommands::StartQuery(Exclude<E...>)
    {
        return Query<entt::exclude_t<E...>, C...>(registry.view<C...>(entt::exclude_t<E...> {}));
    }

    template <typename E>
    void SystemCommands::Broadcast(const E& event)
    {
        host.BroadcastEvent<E>(event);
    }

    template <typename C>
    void ComponentType::EmplaceImpl(Runtime::SystemCommands& commands, Runtime::Entity entity, const Mirror::Any& ref)
    {
        commands.Emplace<C>(entity, ref.As<const C&>());
    }

    template <typename C>
    Mirror::Any ComponentType::GetImpl(Runtime::SystemCommands& commands, Runtime::Entity entity)
    {
        return Mirror::Any(commands.Get<C>(entity));
    }

    template <typename C>
    bool ComponentType::HasImpl(Runtime::SystemCommands& commands, Runtime::Entity entity)
    {
        return commands.Has<C>(entity);
    }

    template <typename C>
    void ComponentType::PatchImpl(Runtime::SystemCommands& commands, Runtime::Entity entity, std::function<void(const Mirror::Any&)> patchFunc)
    {
        commands.Patch<C>(entity, [&patchFunc](C& comp) -> void {
            patchFunc(Mirror::Any(std::ref(comp)));
        });
    }

    template <typename C>
    void ComponentType::SetImpl(Runtime::SystemCommands& commands, Runtime::Entity entity, const Mirror::Any& ref)
    {
        commands.Set<C>(entity, ref.As<const C&>());
    }

    template <typename C>
    void ComponentType::UpdatedImpl(Runtime::SystemCommands& commands, Runtime::Entity entity)
    {
        commands.Updated<C>(entity);
    }

    template <typename C>
    void ComponentType::RemoveImpl(Runtime::SystemCommands& commands, Runtime::Entity entity)
    {
        commands.Remove<C>(entity);
    }

    template <typename S>
    void StateType::EmplaceImpl(Runtime::SystemCommands& commands, const Mirror::Any& ref)
    {
        commands.EmplaceState<S>(ref.As<const S&>());
    }

    template <typename S>
    Mirror::Any StateType::GetImpl(Runtime::SystemCommands& commands)
    {
        return Mirror::Any(commands.GetState<S>());
    }

    template <typename S>
    bool StateType::HasImpl(Runtime::SystemCommands& commands)
    {
        return commands.HasState<S>();
    }

    template <typename S>
    void StateType::PatchImpl(Runtime::SystemCommands& commands, std::function<void(const Mirror::Any&)> patchFunc)
    {
        commands.PatchState<S>([&patchFunc](S& state) -> void {
            patchFunc(Mirror::Any(std::ref(state)));
        });
    }

    template <typename S>
    void StateType::SetImpl(Runtime::SystemCommands& commands, const Mirror::Any& ref)
    {
        commands.SetState<S>(ref.As<const S&>());
    }

    template <typename S>
    void StateType::UpdatedImpl(Runtime::SystemCommands& commands)
    {
        commands.UpdatedState<S>();
    }

    template <typename S>
    void StateType::RemoveImpl(Runtime::SystemCommands& commands)
    {
        commands.RemoveState<S>();
    }

    template <typename S>
    void SystemType::AddSetupSystemImpl(ECSHost& host)
    {
        host.AddSetupSystem<S>();
    }

    template <typename S>
    void SystemType::RemoveSetupSystemImpl(ECSHost& host)
    {
        host.RemoveSetupSystem<S>();
    }

    template <typename S>
    void SystemType::AddTickSystemImpl(ECSHost& host)
    {
        host.AddTickSystem<S>();
    }

    template <typename S>
    void SystemType::RemoveTickSystemImpl(ECSHost& host)
    {
        host.RemoveTickSystem<S>();
    }

    template <typename E, typename S>
    void SystemType::AddEventSystemImpl(ECSHost& host)
    {
        host.AddEventSystem<E, S>();
    }

    template <typename E, typename S>
    void SystemType::RemoveEventSystemImpl(ECSHost& host)
    {
        host.RemoveEventSystem<E, S>();
    }
}
