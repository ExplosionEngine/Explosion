//
// Created by johnk on 2024/10/31.
//

#pragma once

#include <set>
#include <unordered_map>

#include <Common/Event.h>
#include <Common/Utility.h>
#include <Mirror/Mirror.h>

namespace Runtime {
    using Entity = size_t;
}

namespace Runtime::Internal {
    using ArchetypeId = Mirror::TypeId;
    using CompClass = const Mirror::Class*;
    using GlobalCompClass = const Mirror::Class*;
    using ElemPtr = void*;

    template <typename T> const Mirror::Class* GetClass();
    template <typename T> struct LambdaTraits;

    class CompRtti {
    public:
        template <typename C> CompRtti From();

        void Bind(size_t inOffset);
        Mirror::Any MoveConstruct(ElemPtr inElem, const Mirror::Argument& inOther) const;
        Mirror::Any MoveAssign(ElemPtr inElem, const Mirror::Argument& inOther) const;
        void Destruct(ElemPtr inElem) const;
        Mirror::Any Get(ElemPtr inElem) const;
        CompClass Class() const;
        size_t Offset() const;
        size_t Size() const;

    private:
        using MoveConstructFunc = Mirror::Any(ElemPtr, size_t, const Mirror::Argument&);
        using MoveAssignFunc = Mirror::Any(ElemPtr, size_t, const Mirror::Argument&);
        using DestructorFunc = void(ElemPtr, size_t);
        using GetFunc = Mirror::Any(ElemPtr, size_t);

        template <typename T> static Mirror::Any MoveConstructImpl(ElemPtr inElem, size_t inOffset, const Mirror::Argument& inOther);
        template <typename T> static Mirror::Any MoveAssignImpl(ElemPtr inElem, size_t inOffset, const Mirror::Argument& inOther);
        template <typename T> static void DestructImpl(ElemPtr inElem, size_t inOffset);
        template <typename T> static Mirror::Any GetImpl(ElemPtr inElem, size_t inOffset);

        CompRtti();

        CompClass clazz;
        MoveConstructFunc* moveConstruct;
        MoveAssignFunc* moveAssign;
        DestructorFunc* destructor;
        GetFunc* get;
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
        Mirror::Any EmplaceComp(Entity inEntity, CompClass inCompClass, Mirror::Any inComp);
        void EraseElem(Entity inEntity);
        ElemPtr GetElem(Entity inEntity);
        Mirror::Any GetComp(Entity inEntity, CompClass inCompClass);
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
        ElemPtr ElemAt(std::vector<uint8_t>& inMemory, size_t inIndex);
        ElemPtr ElemAt(size_t inIndex);

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
}

namespace Runtime {
    class ECRegistry;

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

    private:
        friend class RuntimeView;

        std::unordered_set<Internal::CompClass> includes;
        std::unordered_set<Internal::CompClass> excludes;
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
        template <typename F, typename ArgTuple, size_t... I> void InvokeTraverseFuncInternal(F&& inFunc, std::pair<Entity, std::vector<void*>>& inEntityAndComps, std::index_sequence<I...>) const;
        template <typename C> decltype(auto) GetCompRef(std::vector<void*>& inComps) const;
        void Evaluate(ECRegistry& inRegistry, const RuntimeViewRule& inArgs);

        std::unordered_map<Internal::CompClass, size_t> slotMap;
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

    class ECRegistry {
    public:
        using EntityTraverseFunc = Internal::EntityPool::EntityTraverseFunc;
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

        // component
        template <typename C, typename... Args> C& Emplace(Entity inEntity, Args&&... inArgs);
        template <typename C> void Remove(Entity inEntity);
        template <typename C> void NotifyUpdated(Entity inEntity) const;
        template <typename C, typename F> void Update(Entity inEntity, F&& inFunc);
        template <typename C> ScopedUpdater<C> Update(Entity inEntity);
        template <typename C> bool Has(Entity inEntity) const;
        template <typename C> C* Find(Entity inEntity);
        template <typename C> const C* Find(Entity inEntity) const;
        template <typename C> C& Get(Entity inEntity);
        template <typename C> const C& Get(Entity inEntity) const;
        template <typename... C, typename... E> View<Exclude<E...>, C...> View(Exclude<E...>);
        template <typename C> CompEvents& Events();
        RuntimeView RuntimeView(const RuntimeViewRule& inRule);
        Observer Observer();

        // global component
        template <typename G, typename... Args> void GEmplace(Args&&... inArgs);
        template <typename G> void GRemove();
        template <typename G> void GNotifyUpdated() const;
        template <typename G, typename F> void GUpdate(F&& inFunc);
        template <typename G> GScopedUpdater<G> GUpdate();
        template <typename G> bool GHas() const;
        template <typename G> G* GFind();
        template <typename G> const G* GFind() const;
        template <typename G> G& GGet();
        template <typename G> const G& GGet() const;
        template <typename G> GCompEvents& GEvents();

    private:
        template <typename... T> friend class View;
        friend class RuntimeView;

        template <typename C> void NotifyConstructed(Entity inEntity) const;
        template <typename C> void NotifyRemove(Entity inEntity) const;
        template <typename G> void GNotifyConstructed() const;
        template <typename G> void GNotifyRemove() const;

        Internal::EntityPool entities;
        std::unordered_map<Internal::GlobalCompClass, Mirror::Any> globalComps;
        std::unordered_map<Internal::ArchetypeId, Internal::Archetype> archetypes;
        std::unordered_map<Internal::CompClass, CompEvents> compEvents;
        std::unordered_map<Internal::GlobalCompClass, GCompEvent> globalCompEvents;
    };

    class SystemRegistry {
    public:

    private:
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

    template <typename C>
    CompRtti CompRtti::From()
    {
        CompRtti result;
        result.clazz = Internal::GetClass<C>();
        result.moveConstruct = &MoveConstructImpl<C>;
        result.moveAssign = &MoveAssignImpl<C>;
        result.destructor = &DestructImpl<C>;
        result.get = &GetImpl<C>;
        result.bound = false;
        result.offset = 0;
        return result;
    }

    template <typename T>
    Mirror::Any CompRtti::MoveConstructImpl(ElemPtr inElem, size_t inOffset, const Mirror::Argument& inOther)
    {
        void* compBegin = static_cast<uint8_t*>(inElem) + inOffset;
        new(compBegin) T(std::move(inOther.As<T&>()));
        return GetImpl<T>(inElem, inOffset);
    }

    template <typename T>
    Mirror::Any CompRtti::MoveAssignImpl(ElemPtr inElem, size_t inOffset, const Mirror::Argument& inOther)
    {
        auto compRef = GetImpl<T>(inElem, inOffset);
        compRef.template As<T&>() = std::move(inOther.As<T&>());
        return compRef;
    }

    template <typename T>
    void CompRtti::DestructImpl(ElemPtr inElem, size_t inOffset)
    {
        auto compRef = GetImpl<T>(inElem, inOffset);
        compRef.template As<T&>().~T();
    }

    template <typename T>
    Mirror::Any CompRtti::GetImpl(ElemPtr inElem, size_t inOffset)
    {
        void* compBegin = static_cast<uint8_t*>(inElem) + inOffset;
        return { std::ref(*static_cast<T*>(compBegin)) };
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
        std::vector<Internal::CompClass> includeCompIds;
        includeCompIds.reserve(sizeof...(C));
        (void) std::initializer_list<int> { ([&]() -> void {
            includeCompIds.emplace_back(Internal::GetClass<C>());
        }(), 0)... };

        std::vector<Internal::CompClass> excludeCompIds;
        excludeCompIds.reserve(sizeof...(E));
        (void) std::initializer_list<int> { ([&]() -> void {
            excludeCompIds.emplace_back(Internal::GetClass<C>());
        }(), 0)... };

        for (auto& [_, archetype] : inRegistry.archetypes) {
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
    void RuntimeView::InvokeTraverseFuncInternal(F&& inFunc, std::pair<Entity, std::vector<void*>>& inEntityAndComps, std::index_sequence<I...>) const
    {
        inFunc(inEntityAndComps.first, GetCompRef<std::tuple_element_t<I, ArgTuple>>(inEntityAndComps.second)...);
    }

    template <typename C>
    decltype(auto) RuntimeView::GetCompRef(std::vector<void*>& inComps) const
    {
        static_assert(std::is_reference_v<C>);
        const auto compIndex = slotMap.at(Internal::GetClass<C>());
        return *static_cast<std::add_pointer_t<std::remove_reference_t<C>>>(inComps[compIndex]);
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
        Assert(Valid(inEntity));
        const auto* clazz = Internal::GetClass<C>();
        const Internal::ArchetypeId archetypeId = entities.GetArchetype(inEntity);
        Internal::Archetype& archetype = archetypes.at(archetypeId);

        const Internal::ArchetypeId newArchetypeId = archetypeId + clazz->GetTypeInfo()->id;
        entities.SetArchetype(inEntity, newArchetypeId);

        Internal::Archetype* newArchetype;
        if (archetypes.contains(newArchetypeId)) {
            newArchetype = &archetypes.at(newArchetypeId);
            newArchetype->EmplaceElem(inEntity, archetype.GetElem(inEntity), archetype.GetRttiVec());
            archetype.EraseElem(inEntity);
        } else {
            archetypes.emplace(newArchetypeId, Internal::Archetype(archetype.NewRttiVecByAdd(Internal::CompRtti::From<C>())));
            newArchetype = &archetypes.at(newArchetypeId);
            newArchetype->EmplaceElem(inEntity);
        }

        C tempObj(std::forward<Args>(inArgs)...);
        auto& result = *static_cast<C*>(newArchetype->EmplaceComp(inEntity, clazz, &tempObj));
        NotifyConstructed<C>(inEntity);
        return result;
    }

    template <typename C>
    void ECRegistry::Remove(Entity inEntity)
    {
        Assert(Valid(inEntity) && Has<C>(inEntity));
        const auto* clazz = Internal::GetClass<C>();
        const Internal::ArchetypeId archetypeId = entities.GetArchetype(inEntity);
        Internal::Archetype& archetype = archetypes.at(archetypeId);

        const Internal::ArchetypeId newArchetypeId = archetypeId - clazz->GetTypeInfo()->id;
        entities.SetArchetype(inEntity, newArchetypeId);

        if (!archetypes.contains(newArchetypeId)) {
            archetypes.emplace(newArchetypeId, Internal::Archetype(archetype.NewRttiVecByRemove(Internal::CompRtti::From<C>())));
        }
        NotifyRemove<C>(inEntity);
        Internal::Archetype& newArchetype = archetypes.at(newArchetypeId);
        newArchetype.EmplaceElem(inEntity, archetype.GetElem(inEntity), archetype.GetRttiVec());
        archetype.EraseElem(inEntity);
    }

    template <typename C, typename F>
    void ECRegistry::Update(Entity inEntity, F&& inFunc)
    {
        Assert(Valid(inEntity) && Has<C>());
        inFunc(Get<C>(inEntity));
        NotifyUpdated<C>(inEntity);
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
        Assert(Valid(inEntity));
        return archetypes
            .at(entities.GetArchetype(inEntity))
            .Contains(Internal::GetClass<C>());
    }

    template <typename C>
    C* ECRegistry::Find(Entity inEntity)
    {
        return Has<C>() ? &Get<C>(inEntity) : nullptr;
    }

    template <typename C>
    const C* ECRegistry::Find(Entity inEntity) const
    {
        return Has<C>() ? &Get<C>(inEntity) : nullptr;
    }

    template <typename C>
    C& ECRegistry::Get(Entity inEntity)
    {
        Assert(Valid(inEntity) && Has<C>());
        void* ptr = archetypes
            .at(entities.GetArchetype(inEntity))
            .GetComp(inEntity, Internal::GetClass<C>());
        return *static_cast<C*>(ptr);
    }

    template <typename C>
    const C& ECRegistry::Get(Entity inEntity) const
    {
        Assert(Valid(inEntity) && Has<C>());
        const void* ptr = archetypes
            .at(entities.GetArchetype(inEntity))
            .GetComp(inEntity, Internal::GetClass<C>());
        return *static_cast<const C*>(ptr);
    }

    template <typename... C, typename... E>
    View<Exclude<E...>, C...> ECRegistry::View(Exclude<E...>)
    {
        return { *this };
    }

    template <typename C>
    ECRegistry::CompEvents& ECRegistry::Events()
    {
        return compEvents[Internal::GetClass<C>()];
    }

    template <typename C>
    void ECRegistry::NotifyUpdated(Entity inEntity) const
    {
        const auto iter = compEvents.find(Internal::GetClass<C>());
        if (iter == compEvents.end()) {
            return;
        }
        iter->second.onUpdated.Broadcast(inEntity);
    }

    template <typename C>
    void ECRegistry::NotifyConstructed(Entity inEntity) const
    {
        const auto iter = compEvents.find(Internal::GetClass<C>());
        if (iter == compEvents.end()) {
            return;
        }
        iter->second.onConstructed.Broadcast(inEntity);
    }

    template <typename C>
    void ECRegistry::NotifyRemove(Entity inEntity) const
    {
        const auto iter = compEvents.find(Internal::GetClass<C>());
        if (iter == compEvents.end()) {
            return;
        }
        iter->second.onRemove.Broadcast(inEntity);
    }

    template <typename G, typename ... Args>
    void ECRegistry::GEmplace(Args&&... inArgs)
    {
        Assert(!GHas<G>());
        globalComps.emplace(Internal::GetClass<G>(), Mirror::Any(G(std::forward<Args>(inArgs))...));
        GNotifyConstructed<G>();
    }

    template <typename G>
    void ECRegistry::GRemove()
    {
        Assert(GHas<G>());
        GNotifyRemove<G>();
        globalComps.erase(Internal::GetClass<G>());
    }

    template <typename G, typename F>
    void ECRegistry::GUpdate(F&& inFunc)
    {
        Assert(GHas<G>());
        inFunc(GGet<G>());
        GNotifyUpdated<G>();
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
        return globalComps.contains(Internal::GetClass<G>());
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
        Assert(GHas<G>());
        return globalComps.at(Internal::GetClass<G>()).template As<G&>();
    }

    template <typename G>
    const G& ECRegistry::GGet() const
    {
        Assert(GHas<G>());
        return globalComps.at(Internal::GetClass<G>()).template As<const G&>();
    }

    template <typename G>
    ECRegistry::GCompEvents& ECRegistry::GEvents()
    {
        return globalCompEvents[Internal::GetClass<G>()];
    }

    template <typename G>
    void ECRegistry::GNotifyUpdated() const
    {
        const auto iter = globalCompEvents.find(Internal::GetClass<G>());
        if (iter == globalCompEvents.end()) {
            return;
        }
        iter->second.onUpdated.Broadcast();
    }

    template <typename G>
    void ECRegistry::GNotifyConstructed() const
    {
        const auto iter = globalCompEvents.find(Internal::GetClass<G>());
        if (iter == globalCompEvents.end()) {
            return;
        }
        iter->second.onConstructed.Broadcast();
    }

    template <typename G>
    void ECRegistry::GNotifyRemove() const
    {
        const auto iter = globalCompEvents.find(Internal::GetClass<G>());
        if (iter == globalCompEvents.end()) {
            return;
        }
        iter->second.onRemove.Broadcast();
    }
} // namespace Runtime
