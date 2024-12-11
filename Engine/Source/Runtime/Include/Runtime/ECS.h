//
// Created by johnk on 2024/10/31.
//

#pragma once

#include <set>
#include <unordered_map>

#include <Common/Event.h>
#include <Common/Utility.h>
#include <Common/Memory.h>
#include <Mirror/Mirror.h>
#include <Mirror/Meta.h>

namespace Runtime {
    using Entity = size_t;
    static constexpr Entity entityNull = 0;

    using CompClass = const Mirror::Class*;
    using GCompClass = const Mirror::Class*;
    using SystemClass = const Mirror::Class*;

    class ECRegistry;

    class EClass() System {
    public:
        EClassBody(System)

        explicit System(ECRegistry& inRegistry);
        virtual ~System();
        virtual void Execute(float inDeltaTimeMs);

    protected:
        ECRegistry& registry;
    };
}

namespace Runtime::Internal {
    using ArchetypeId = Mirror::TypeId;
    using ElemPtr = void*;

    template <typename T> const Mirror::Class* GetClass();
    template <typename T> struct LambdaTraits;

    class CompRtti {
    public:
        explicit CompRtti(CompClass inClass);
        void Bind(size_t inOffset);
        Mirror::Any MoveConstruct(ElemPtr inElem, const Mirror::Argument& inOther) const;
        Mirror::Any MoveAssign(ElemPtr inElem, const Mirror::Argument& inOther) const;
        void Destruct(ElemPtr inElem) const;
        Mirror::Any Get(ElemPtr inElem) const;
        CompClass Class() const;
        size_t Offset() const;
        size_t Size() const;

    private:
        using MoveConstructFunc = Mirror::Any(ElemPtr, size_t, const Mirror::Any&);
        using MoveAssignFunc = Mirror::Any(ElemPtr, size_t, const Mirror::Any&);
        using DestructorFunc = void(ElemPtr, size_t);
        using GetFunc = Mirror::Any(ElemPtr, size_t);

        CompClass clazz;
        // runtime, need Bind()
        bool bound;
        size_t offset;
    };

    class Archetype {
    public:
        explicit Archetype(const std::vector<CompRtti>& inRttiVec);

        bool Contains(CompClass inClazz) const;
        bool ContainsAll(const std::vector<CompClass>& inClasses) const;
        bool NotContainsAny(const std::vector<CompClass>& inCompClasses) const;
        ElemPtr EmplaceElem(Entity inEntity);
        ElemPtr EmplaceElem(Entity inEntity, ElemPtr inSrcElem, const std::vector<CompRtti>& inSrcRttiVec);
        Mirror::Any EmplaceComp(Entity inEntity, CompClass inCompClass, const Mirror::Argument& inCompRef);
        void EraseElem(Entity inEntity);
        ElemPtr GetElem(Entity inEntity) const;
        Mirror::Any GetComp(Entity inEntity, CompClass inCompClass);
        Mirror::Any GetComp(Entity inEntity, CompClass inCompClass) const;
        size_t Size() const;
        auto All() const;
        const std::vector<CompRtti>& GetRttiVec() const;
        ArchetypeId Id() const;
        std::vector<CompRtti> NewRttiVecByAdd(const CompRtti& inRtti) const;
        std::vector<CompRtti> NewRttiVecByRemove(const CompRtti& inRtti) const;

    private:
        using CompRttiIndex = size_t;
        using ElemIndex = size_t;

        const CompRtti* FindCompRtti(CompClass clazz) const;
        const CompRtti& GetCompRtti(CompClass clazz) const;
        size_t Capacity() const;
        void Reserve(float inRatio = 1.5f);
        ElemPtr AllocateNewElemBack();
        ElemPtr ElemAt(std::vector<uint8_t>& inMemory, size_t inIndex) const;
        ElemPtr ElemAt(size_t inIndex) const;

        ArchetypeId id;
        size_t size;
        size_t elemSize;
        std::vector<CompRtti> rttiVec;
        std::unordered_map<CompClass, CompRttiIndex> rttiMap;
        std::unordered_map<Entity, ElemIndex> entityMap;
        std::unordered_map<ElemIndex, Entity> elemMap;
        std::vector<uint8_t> memory;
    };

    class EntityPool {
    public:
        using EntityTraverseFunc = std::function<void(Entity)>;
        using ConstIter = std::set<Entity>::const_iterator;

        EntityPool();

        size_t Size() const;
        bool Valid(Entity inEntity) const;
        Entity Allocate();
        void Free(Entity inEntity);
        void Clear();
        void Each(const EntityTraverseFunc& inFunc) const;
        void SetArchetype(Entity inEntity, ArchetypeId inArchetypeId);
        ArchetypeId GetArchetype(Entity inEntity) const;
        ConstIter Begin() const;
        ConstIter End() const;

    private:
        size_t counter;
        std::set<Entity> free;
        std::set<Entity> allocated;
        std::unordered_map<Entity, ArchetypeId> archetypeMap;
    };

    class SystemFactory {
    public:
        explicit SystemFactory(SystemClass inClass);
        Common::UniqueRef<System> Build(ECRegistry& inRegistry) const;
        std::unordered_map<std::string, Mirror::Any> GetArguments();
        const std::unordered_map<std::string, Mirror::Any>& GetArguments() const;

    private:
        void BuildArgumentLists();

        SystemClass clazz;
        std::unordered_map<std::string, Mirror::Any> arguments;
    };
}

namespace Runtime {
    template <typename C>
    class ScopedUpdater {
    public:
        ScopedUpdater(ECRegistry& inRegistry, Entity inEntity, C& inCompRef);
        ~ScopedUpdater();

        NonCopyable(ScopedUpdater)
        NonMovable(ScopedUpdater)

        C* operator->() const;

    private:
        ECRegistry& registry;
        Entity entity;
        C& compRef;
    };

    template <typename G>
    class GScopedUpdater {
    public:
        GScopedUpdater(ECRegistry& inRegistry, G& inGlobalCompRef);
        ~GScopedUpdater();

        NonCopyable(GScopedUpdater)
        NonMovable(GScopedUpdater)

        G* operator->() const;

    private:
        ECRegistry& registry;
        G& globalCompRef;
    };

    class ScopedUpdaterDyn {
    public:
        ScopedUpdaterDyn(ECRegistry& inRegistry, CompClass inClass, Entity inEntity, const Mirror::Any& inCompRef);
        ~ScopedUpdaterDyn();

        NonCopyable(ScopedUpdaterDyn)
        NonMovable(ScopedUpdaterDyn)

        template <typename T> T As() const;

    private:
        ECRegistry& registry;
        CompClass clazz;
        Entity entity;
        const Mirror::Any& compRef;
    };

    class GScopedUpdaterDyn {
    public:
        GScopedUpdaterDyn(ECRegistry& inRegistry, GCompClass inClass, const Mirror::Any& inGlobalCompRef);
        ~GScopedUpdaterDyn();

        NonCopyable(GScopedUpdaterDyn)
        NonMovable(GScopedUpdaterDyn)

        template <typename T> T As() const;

    private:
        ECRegistry& registry;
        GCompClass clazz;
        const Mirror::Any& globalCompRef;
    };

    template <typename... T>
    struct Contains {};

    template <typename... T>
    struct Exclude {};

    template <typename... T>
    class View;

    template <typename... C, typename... E>
    class View<Exclude<E...>, C...> {
    public:
        explicit View(ECRegistry& inRegistry);
        NonCopyable(View)
        NonMovable(View)

        template <typename F> void Each(F&& inFunc) const;
        auto Begin();
        auto Begin() const;
        auto End();
        auto End() const;
        auto begin();
        auto begin() const;
        auto end();
        auto end() const;

    private:
        void Evaluate(ECRegistry& inRegistry);

        std::vector<std::tuple<Entity, C&...>> result;
    };

    class RuntimeViewRule {
    public:
        RuntimeViewRule();
        template <typename C> RuntimeViewRule& Include();
        template <typename C> RuntimeViewRule& Exclude();
        RuntimeViewRule& IncludeDyn(CompClass inClass);
        RuntimeViewRule& ExcludeDyn(CompClass inClass);

    private:
        friend class RuntimeView;

        std::unordered_set<CompClass> includes;
        std::unordered_set<CompClass> excludes;
    };

    class RuntimeView {
    public:
        explicit RuntimeView(ECRegistry& inRegistry, const RuntimeViewRule& inArgs);
        NonCopyable(RuntimeView)
        NonMovable(RuntimeView)

        template <typename F> void Each(F&& inFunc) const;
        auto Begin();
        auto Begin() const;
        auto End();
        auto End() const;
        auto begin();
        auto begin() const;
        auto end();
        auto end() const;

    private:
        template <typename F, typename ArgTuple, size_t... I> void InvokeTraverseFuncInternal(F&& inFunc, std::pair<Entity, std::vector<Mirror::Any>>& inEntityAndComps, std::index_sequence<I...>) const;
        template <typename C> decltype(auto) GetCompRef(std::vector<Mirror::Any>& inComps) const;
        void Evaluate(ECRegistry& inRegistry, const RuntimeViewRule& inArgs);

        std::unordered_map<CompClass, size_t> slotMap;
        std::vector<Entity> resultEntities;
        std::vector<std::pair<Entity, std::vector<Mirror::Any>>> result;
    };

    class Observer {
    public:
        using ConstIter = std::vector<Entity>::const_iterator;
        using EntityTraverseFunc = Internal::EntityPool::EntityTraverseFunc;
        using ReceiverDeleter = std::function<void()>;

        explicit Observer(ECRegistry& inRegistry);
        ~Observer();
        NonCopyable(Observer)
        NonMovable(Observer)

        template <typename C> void ObConstructed();
        template <typename C> void ObUpdated();
        template <typename C> void ObRemove();
        void Each(const EntityTraverseFunc& inFunc) const;
        void Clear();
        void Reset();
        ConstIter Begin() const;
        ConstIter End() const;
        ConstIter begin() const;
        ConstIter end() const;

    private:
        void OnEvent(Common::Event<ECRegistry&, Entity>& inEvent);
        void RecordEntity(ECRegistry& inRegistry, Entity inEntity);

        ECRegistry& registry;
        std::vector<std::pair<Common::ReceiverHandle, ReceiverDeleter>> receiverHandles;
        std::vector<Entity> entities;
    };

    class MIRROR_API ECRegistry {
    public:
        using EntityTraverseFunc = Internal::EntityPool::EntityTraverseFunc;
        using DynUpdateFunc = std::function<void(const Mirror::Any&)>;
        using ConstIter = Internal::EntityPool::ConstIter;
        using CompEvent = Common::Event<ECRegistry&, Entity>;
        using GCompEvent = Common::Event<ECRegistry&>;

        struct CompEvents {
            CompEvent onConstructed;
            CompEvent onUpdated;
            CompEvent onRemove;
        };

        struct GCompEvents {
            GCompEvent onConstructed;
            GCompEvent onUpdated;
            GCompEvent onRemove;
        };

        ECRegistry();
        ~ECRegistry();

        ECRegistry(const ECRegistry& inOther);
        ECRegistry(ECRegistry&& inOther) noexcept;
        ECRegistry& operator=(const ECRegistry& inOther);
        ECRegistry& operator=(ECRegistry&& inOther) noexcept;

        void ResetTransients();

        // entity
        Entity Create();
        void Destroy(Entity inEntity);
        bool Valid(Entity inEntity) const;
        size_t Size() const;
        void Clear();
        void Each(const EntityTraverseFunc& inFunc) const;
        ConstIter Begin() const;
        ConstIter End() const;
        ConstIter begin() const;
        ConstIter end() const;

        // component static
        template <typename C, typename... Args> C& Emplace(Entity inEntity, Args&&... inArgs);
        template <typename C> void Remove(Entity inEntity);
        template <typename C> void NotifyUpdated(Entity inEntity);
        template <typename C, typename F> void Update(Entity inEntity, F&& inFunc);
        template <typename C> ScopedUpdater<C> Update(Entity inEntity);
        template <typename C> bool Has(Entity inEntity) const;
        template <typename C> C* Find(Entity inEntity);
        template <typename C> const C* Find(Entity inEntity) const;
        template <typename C> C& Get(Entity inEntity);
        template <typename C> const C& Get(Entity inEntity) const;
        template <typename... C, typename... E> View<Exclude<E...>, C...> View(Exclude<E...>);
        template <typename C> CompEvents& Events();
        Observer Observer();

        // component dynamic
        Mirror::Any EmplaceDyn(CompClass inClass, Entity inEntity, const Mirror::ArgumentList& inArgs);
        void RemoveDyn(CompClass inClass, Entity inEntity);
        void NotifyUpdatedDyn(CompClass inClass, Entity inEntity);
        void UpdateDyn(CompClass inClass, Entity inEntity, const DynUpdateFunc& inFunc);
        ScopedUpdaterDyn UpdateDyn(CompClass inClass, Entity inEntity);
        bool HasDyn(CompClass inClass, Entity inEntity) const;
        Mirror::Any FindDyn(CompClass inClass, Entity inEntity);
        Mirror::Any FindDyn(CompClass inClass, Entity inEntity) const;
        Mirror::Any GetDyn(CompClass inClass, Entity inEntity);
        Mirror::Any GetDyn(CompClass inClass, Entity inEntity) const;
        CompEvents& EventsDyn(CompClass inClass);
        RuntimeView RuntimeView(const RuntimeViewRule& inRule);

        // global component static
        template <typename G, typename... Args> G& GEmplace(Args&&... inArgs);
        template <typename G> void GRemove();
        template <typename G> void GNotifyUpdated();
        template <typename G, typename F> void GUpdate(F&& inFunc);
        template <typename G> GScopedUpdater<G> GUpdate();
        template <typename G> bool GHas() const;
        template <typename G> G* GFind();
        template <typename G> const G* GFind() const;
        template <typename G> G& GGet();
        template <typename G> const G& GGet() const;
        template <typename G> GCompEvents& GEvents();

        // global component dynamic
        Mirror::Any GEmplaceDyn(GCompClass inClass, const Mirror::ArgumentList& inArgs);
        void GRemoveDyn(GCompClass inClass);
        void GNotifyUpdatedDyn(GCompClass inClass);
        void GUpdateDyn(GCompClass inClass, const DynUpdateFunc& inFunc);
        GScopedUpdaterDyn GUpdateDyn(GCompClass inClass);
        bool GHasDyn(GCompClass inClass) const;
        Mirror::Any GFindDyn(GCompClass inClass);
        Mirror::Any GFindDyn(GCompClass inClass) const;
        Mirror::Any GGetDyn(GCompClass inClass);
        Mirror::Any GGetDyn(GCompClass inClass) const;
        GCompEvents& GEventsDyn(GCompClass inClass);

    private:
        template <typename... T> friend class View;
        friend class RuntimeView;

        template <typename C> void NotifyConstructed(Entity inEntity);
        template <typename C> void NotifyRemove(Entity inEntity);
        template <typename G> void GNotifyConstructed();
        template <typename G> void GNotifyRemove();
        void NotifyConstructedDyn(CompClass inClass, Entity inEntity);
        void NotifyRemoveDyn(CompClass inClass, Entity inEntity);
        void GNotifyConstructedDyn(GCompClass inClass);
        void GNotifyRemoveDyn(CompClass inClass);

        Internal::EntityPool entities;
        std::unordered_map<GCompClass, Mirror::Any> globalComps;
        std::unordered_map<Internal::ArchetypeId, Internal::Archetype> archetypes;
        // transients
        std::unordered_map<CompClass, CompEvents> compEvents;
        std::unordered_map<GCompClass, GCompEvents> globalCompEvents;
    };

    class SystemGroup {
    public:
        explicit SystemGroup(std::string inName);

        Internal::SystemFactory& EmplaceSystem(SystemClass inClass);
        void RemoveSystem(SystemClass inClass);
        bool HasSystem(SystemClass inClass) const;
        Internal::SystemFactory& GetSystem(SystemClass inClass);
        const Internal::SystemFactory& GetSystem(SystemClass inClass) const;
        auto GetSystems();
        auto GetSystems() const;
        const std::string& GetName() const;

    private:
        std::string name;
        std::unordered_map<SystemClass, Internal::SystemFactory> systems;
    };

    class SystemGraph {
    public:
        SystemGraph();

        SystemGroup& AddGroup(const std::string& inName);
        void RemoveGroup(const std::string& inName);
        bool HasGroup(const std::string& inName) const;
        SystemGroup& GetGroup(const std::string& inName);
        const SystemGroup& GetGroup(const std::string& inName) const;
        const std::vector<SystemGroup>& GetGroups() const;

    private:
        std::vector<SystemGroup> systemGroups;
    };

    class SystemPipeline {
    public:
        explicit SystemPipeline(const SystemGraph& inGraph);

    private:
        struct SystemContext {
            const Internal::SystemFactory& factory;
            Common::UniqueRef<System> instance;
        };
        using ActionFunc = std::function<void(SystemContext&)>;

        void ParallelPerformAction(const ActionFunc& inActionFunc);

        friend class SystemGraphExecutor;

        std::vector<std::vector<SystemContext>> systemGraph;
    };

    class SystemGraphExecutor {
    public:
        explicit SystemGraphExecutor(ECRegistry& inEcRegistry, const SystemGraph& inSystemGraph);
        ~SystemGraphExecutor();

        NonCopyable(SystemGraphExecutor)
        NonMovable(SystemGraphExecutor)

        void Tick(float inDeltaTimeMs);

    private:
        ECRegistry& ecRegistry;
        SystemGraph systemGraph;
        SystemPipeline pipeline;
    };
}

namespace Runtime::Internal {
    template <typename T>
    const Mirror::Class* GetClass()
    {
        return &Mirror::Class::Get<T>();
    }

    template <typename Ret, typename... T>
    struct LambdaTraits<std::function<Ret(T...)>> {
        static constexpr auto ArgSize = sizeof...(T);
        using ArgsTupleType = std::tuple<T...>;
    };

    inline auto Archetype::All() const
    {
        return elemMap | std::ranges::views::values;
    }
} // namespace Runtime::Internal

namespace Runtime {
    template <typename C>
    ScopedUpdater<C>::ScopedUpdater(ECRegistry& inRegistry, Entity inEntity, C& inCompRef)
        : registry(inRegistry)
        , entity(inEntity)
        , compRef(inCompRef)
    {
    }

    template <typename C>
    ScopedUpdater<C>::~ScopedUpdater()
    {
        registry.NotifyUpdated<C>(entity);
    }

    template <typename C>
    C* ScopedUpdater<C>::operator->() const
    {
        return &compRef;
    }

    template <typename G>
    GScopedUpdater<G>::GScopedUpdater(ECRegistry& inRegistry, G& inGlobalCompRef)
        : registry(inRegistry)
        , globalCompRef(inGlobalCompRef)
    {
    }

    template <typename G>
    GScopedUpdater<G>::~GScopedUpdater()
    {
        registry.GNotifyUpdated<G>();
    }

    template <typename G>
    G* GScopedUpdater<G>::operator->() const
    {
        return &globalCompRef;
    }

    template <typename T>
    T ScopedUpdaterDyn::As() const
    {
        return compRef.As<T>();
    }

    template <typename T>
    T GScopedUpdaterDyn::As() const
    {
        return globalCompRef.As<T>();
    }

    template <typename... C, typename... E>
    View<Exclude<E...>, C...>::View(ECRegistry& inRegistry)
    {
        Evaluate(inRegistry);
    }

    template <typename... C, typename... E>
    template <typename F>
    void View<Exclude<E...>, C...>::Each(F&& inFunc) const
    {
        for (const auto& entity : result) {
            std::apply(inFunc, entity);
        }
    }

    template <typename... C, typename... E>
    auto View<Exclude<E...>, C...>::Begin()
    {
        return result.begin();
    }

    template <typename... C, typename... E>
    auto View<Exclude<E...>, C...>::Begin() const
    {
        return result.begin();
    }

    template <typename... C, typename... E>
    auto View<Exclude<E...>, C...>::End()
    {
        return result.end();
    }

    template <typename... C, typename... E>
    auto View<Exclude<E...>, C...>::End() const
    {
        return result.end();
    }

    template <typename... C, typename... E>
    auto View<Exclude<E...>, C...>::begin()
    {
        return Begin();
    }

    template <typename... C, typename... E>
    auto View<Exclude<E...>, C...>::begin() const
    {
        return Begin();
    }

    template <typename... C, typename... E>
    auto View<Exclude<E...>, C...>::end()
    {
        return End();
    }

    template <typename... C, typename... E>
    auto View<Exclude<E...>, C...>::end() const
    {
        return End();
    }

    template <typename... C, typename... E>
    void View<Exclude<E...>, C...>::Evaluate(ECRegistry& inRegistry)
    {
        std::vector<CompClass> includeCompIds;
        includeCompIds.reserve(sizeof...(C));
        (void) std::initializer_list<int> { ([&]() -> void {
            includeCompIds.emplace_back(Internal::GetClass<C>());
        }(), 0)... };

        std::vector<CompClass> excludeCompIds;
        excludeCompIds.reserve(sizeof...(E));
        (void) std::initializer_list<int> { ([&]() -> void {
            excludeCompIds.emplace_back(Internal::GetClass<C>());
        }(), 0)... };

        for (auto& archetype : inRegistry.archetypes | std::views::values) {
            if (!archetype.ContainsAll(includeCompIds) || !archetype.NotContainsAny(excludeCompIds)) {
                continue;
            }

            result.reserve(result.size() + archetype.Size());
            for (auto entity : archetype.All()) {
                result.emplace_back(entity, inRegistry.Get<C>(entity)...);
            }
        }
    }

    template <typename F>
    void RuntimeView::Each(F&& inFunc) const
    {
        for (const auto& pair : result) {
            InvokeTraverseFuncInternal<F, Internal::LambdaTraits<F>::ArgsTupleType>(std::forward<F>(inFunc), pair, std::make_index_sequence<Internal::LambdaTraits<F>::ArgSize> {});
        }
    }

    template <typename F, typename ArgTuple, size_t... I>
    void RuntimeView::InvokeTraverseFuncInternal(F&& inFunc, std::pair<Entity, std::vector<Mirror::Any>>& inEntityAndComps, std::index_sequence<I...>) const
    {
        inFunc(inEntityAndComps.first, GetCompRef<std::tuple_element_t<I, ArgTuple>>(inEntityAndComps.second)...);
    }

    template <typename C>
    decltype(auto) RuntimeView::GetCompRef(std::vector<Mirror::Any>& inComps) const
    {
        static_assert(std::is_reference_v<C>);
        const auto compIndex = slotMap.at(Internal::GetClass<C>());
        return inComps[compIndex].template As<C>();
    }

    template <typename C>
    RuntimeViewRule& RuntimeViewRule::Include()
    {
        const auto* clazz = Internal::GetClass<C>();
        Assert(!includes.contains(clazz));
        includes.emplace(clazz);
        return *this;
    }

    template <typename C>
    RuntimeViewRule& RuntimeViewRule::Exclude()
    {
        const auto* clazz = Internal::GetClass<C>();
        Assert(!excludes.contains(clazz));
        excludes.emplace(clazz);
        return *this;
    }

    template <typename C>
    void Observer::ObConstructed()
    {
        OnEvent(registry.Events<C>().onConstructed);
    }

    template <typename C>
    void Observer::ObUpdated()
    {
        OnEvent(registry.Events<C>().onUpdated);
    }

    template <typename C>
    void Observer::ObRemove()
    {
        OnEvent(registry.Events<C>().onRemove);
    }

    template <typename C, typename ... Args>
    C& ECRegistry::Emplace(Entity inEntity, Args&&... inArgs)
    {
        return EmplaceDyn(Internal::GetClass<C>(), inEntity, Mirror::ForwardAsArgList(std::forward<Args>(inArgs)...)).template As<C&>();
    }

    template <typename C>
    void ECRegistry::Remove(Entity inEntity)
    {
        RemoveDyn(Internal::GetClass<C>(), inEntity);
    }

    template <typename C, typename F>
    void ECRegistry::Update(Entity inEntity, F&& inFunc)
    {
        UpdateDyn(Internal::GetClass<C>(), inEntity, std::forward<F>(inFunc));
    }

    template <typename C>
    ScopedUpdater<C> ECRegistry::Update(Entity inEntity)
    {
        Assert(Valid(inEntity) && Has<C>());
        return { *this, inEntity, Get<C>(inEntity) };
    }

    template <typename C>
    bool ECRegistry::Has(Entity inEntity) const
    {
        return HasDyn(Internal::GetClass<C>(), inEntity);
    }

    template <typename C>
    C* ECRegistry::Find(Entity inEntity)
    {
        return Has<C>(inEntity) ? &Get<C>(inEntity) : nullptr;
    }

    template <typename C>
    const C* ECRegistry::Find(Entity inEntity) const
    {
        return Has<C>(inEntity) ? &Get<C>(inEntity) : nullptr;
    }

    template <typename C>
    C& ECRegistry::Get(Entity inEntity)
    {
        return GetDyn(Internal::GetClass<C>(), inEntity).template As<C&>();
    }

    template <typename C>
    const C& ECRegistry::Get(Entity inEntity) const
    {
        return GetDyn(Internal::GetClass<C>(), inEntity).template As<const C&>();
    }

    template <typename... C, typename... E>
    View<Exclude<E...>, C...> ECRegistry::View(Exclude<E...>)
    {
        return { *this };
    }

    template <typename C>
    ECRegistry::CompEvents& ECRegistry::Events()
    {
        return EventsDyn(Internal::GetClass<C>());
    }

    template <typename C>
    void ECRegistry::NotifyUpdated(Entity inEntity)
    {
        NotifyUpdatedDyn(Internal::GetClass<C>(), inEntity);
    }

    template <typename C>
    void ECRegistry::NotifyConstructed(Entity inEntity)
    {
        NotifyConstructedDyn(Internal::GetClass<C>(), inEntity);
    }

    template <typename C>
    void ECRegistry::NotifyRemove(Entity inEntity)
    {
        NotifyRemoveDyn(Internal::GetClass<C>(), inEntity);
    }

    template <typename G, typename ... Args>
    G& ECRegistry::GEmplace(Args&&... inArgs)
    {
        return GEmplaceDyn(Internal::GetClass<G>(), Mirror::ForwardAsArgList(std::forward<Args>(inArgs)...));
    }

    template <typename G>
    void ECRegistry::GRemove()
    {
        return GRemoveDyn(Internal::GetClass<G>());
    }

    template <typename G, typename F>
    void ECRegistry::GUpdate(F&& inFunc)
    {
        GUpdateDyn(Internal::GetClass<G>(), std::forward<F>(inFunc));
    }

    template <typename G>
    GScopedUpdater<G> ECRegistry::GUpdate()
    {
        Assert(GHas<G>());
        return { *this, GGet<G>() };
    }

    template <typename G>
    bool ECRegistry::GHas() const
    {
        return GHasDyn(Internal::GetClass<G>());
    }

    template <typename G>
    G* ECRegistry::GFind()
    {
        return GHas<G>() ? &GGet<G>() : nullptr;
    }

    template <typename G>
    const G* ECRegistry::GFind() const
    {
        return GHas<G>() ? &GGet<G>() : nullptr;
    }

    template <typename G>
    G& ECRegistry::GGet()
    {
        return GGetDyn(Internal::GetClass<G>()).template As<G&>();
    }

    template <typename G>
    const G& ECRegistry::GGet() const
    {
        return GGetDyn(Internal::GetClass<G>()).template As<const G&>();
    }

    template <typename G>
    ECRegistry::GCompEvents& ECRegistry::GEvents()
    {
        return GEventsDyn(Internal::GetClass<G>());
    }

    template <typename G>
    void ECRegistry::GNotifyUpdated()
    {
        GNotifyUpdatedDyn(Internal::GetClass<G>());
    }

    template <typename G>
    void ECRegistry::GNotifyConstructed()
    {
        GNotifyConstructedDyn(Internal::GetClass<G>());
    }

    template <typename G>
    void ECRegistry::GNotifyRemove()
    {
        GNotifyRemoveDyn(Internal::GetClass<G>());
    }
} // namespace Runtime
