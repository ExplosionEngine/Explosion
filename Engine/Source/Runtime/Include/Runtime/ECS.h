//
// Created by johnk on 2024/10/31.
//

#pragma once

#include <set>
#include <unordered_map>

#include <Common/Event.h>
#include <Common/Utility.h>

namespace Runtime {
    using Entity = size_t;
}

namespace Runtime::Internal {
    using ArchetypeId = size_t;
    using CompId = size_t;

    class CompOp {
    public:
        template <typename C> static CompOp From();

        void SetOffset(size_t inOffset);
        void* Emplace(void* inElem, void* inOther) const;
        void Destruct(void* inElem) const;
        void* Get(void* inElem) const;
        CompId GetCompId() const;
        size_t GetOffset() const;
        size_t GetSize() const;

    private:
        using EmplaceFunc = void*(void*, size_t, void*);
        using DestructFunc = void(void*, size_t);
        using GetFunc = void*(void*, size_t);

        template <typename T> static void* EmplaceImpl(void* inElem, size_t inOffset, void* inOther);
        template <typename T> static void DestructImpl(void* inElem, size_t inOffset);
        template <typename T> static void* GetImpl(void* inElem, size_t inOffset);

        CompOp();

        CompId compId;
        size_t offset;
        size_t size;
        EmplaceFunc* emplace;
        DestructFunc* destruct;
        GetFunc* get;
    };

    class Archetype {
    public:
        explicit Archetype(const std::vector<CompOp>& inCompOps);

        bool Contains(CompId inCompId) const;
        bool ContainsAll(const std::vector<CompId>& inCompIds) const;
        bool NotContainsAny(const std::vector<CompId>& inCompIds) const;
        void* Emplace(Entity inEntity);
        void* Emplace(Entity inEntity, void* inSourceElem, const std::vector<CompOp>& inSourceCompOps);
        void* EmplaceComp(Entity inEntity, CompId inCompId, void* inComp);
        void Erase(Entity inEntity);
        void* Get(Entity inEntity);
        void* GetComp(Entity inEntity, CompId inCompId);
        size_t Size() const;
        std::vector<Entity> All() const;
        const std::vector<CompOp>& GetCompOps() const;
        ArchetypeId GetArchetypeId() const;
        std::vector<CompOp> NewArchetypeAddComp(const CompOp& inOp);
        std::vector<CompOp> NewArchetypeRemoveComp(const CompOp& inOp);

    private:
        using ElemIndex = size_t;

        const CompOp* FindCompOp(CompId inCompId) const;
        size_t Capacity() const;
        void Reserve(float inRatio = 1.5f);
        void* AllocateNewElemBack();
        ElemIndex BackElemIndex() const;
        void* GetElement(size_t inIndex);

        ArchetypeId archetypeId;
        size_t size;
        size_t elemSize;
        std::vector<CompOp> compOps;
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

    template <typename... T>
    struct Contains {};

    template <typename... T>
    struct Exclude {};

    template <typename... T>
    class View;

    template <typename... C, typename... E>
    class View<Exclude<E...>, C...> {
    public:
        using Iter = typename std::vector<std::tuple<Entity, C&...>>::iterator;
        using ConstIter = typename std::vector<std::tuple<Entity, C&...>>::const_iterator;

        explicit View(ECRegistry& inRegistry);
        NonCopyable(View)
        NonMovable(View)

        template <typename F> void Each(F&& inFunc) const;
        Iter Begin();
        ConstIter Begin() const;
        Iter End();
        ConstIter End() const;
        Iter begin();
        ConstIter begin() const;
        Iter end();
        ConstIter end() const;

    private:
        void Evaluate(ECRegistry& inRegistry);

        std::vector<std::tuple<Entity, C&...>> result;
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
        struct CompEvents {
            CompEvent onConstructed;
            CompEvent onUpdated;
            CompEvent onRemove;
        };

        ECRegistry();
        ~ECRegistry();

        ECRegistry(const ECRegistry& inOther);
        ECRegistry(ECRegistry&& inOther) noexcept;
        ECRegistry& operator=(const ECRegistry& inOther);
        ECRegistry& operator=(ECRegistry&& inOther) noexcept;

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
        Observer Observer();
        // TODO states

    private:
        template <typename... T> friend class View;

        template <typename C> void NotifyConstructed(Entity inEntity) const;
        template <typename C> void NotifyRemove(Entity inEntity) const;

        Internal::EntityPool entities;
        std::unordered_map<Internal::ArchetypeId, Internal::Archetype> archetypes;
        std::unordered_map<Internal::CompId, CompEvents> compEvents;
    };

    class SystemRegistry {
    public:

    private:
    };
}

namespace Runtime::Internal {
    template <typename C>
    CompOp CompOp::From()
    {
        CompOp result;
        result.compId = typeid(C).hash_code();
        result.offset = 0;
        result.size = sizeof(C);
        result.emplace = &EmplaceImpl<C>;
        result.destruct = &DestructImpl<C>;
        result.get = &GetImpl<C>;
        return result;
    }

    template <typename T>
    void* CompOp::EmplaceImpl(void* inElem, size_t inOffset, void* inOther)
    {
        void* compBegin = static_cast<uint8_t*>(inElem) + inOffset;
        new(compBegin) T(std::move(*static_cast<T*>(inOffset)));
        return compBegin;
    }

    template <typename T>
    void CompOp::DestructImpl(void* inElem, size_t inOffset)
    {
        void* compBegin = static_cast<uint8_t*>(inElem) + inOffset;
        *static_cast<T*>(compBegin).~T();
    }

    template <typename T>
    void* CompOp::GetImpl(void* inElem, size_t inOffset)
    {
        return static_cast<uint8_t*>(inElem) + inOffset;
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
    typename View<Exclude<E...>, C...>::Iter View<Exclude<E...>, C...>::Begin()
    {
        return result.begin();
    }

    template <typename... C, typename... E>
    typename View<Exclude<E...>, C...>::ConstIter View<Exclude<E...>, C...>::Begin() const
    {
        return result.begin();
    }

    template <typename... C, typename... E>
    typename View<Exclude<E...>, C...>::Iter View<Exclude<E...>, C...>::End()
    {
        return result.end();
    }

    template <typename... C, typename... E>
    typename View<Exclude<E...>, C...>::ConstIter View<Exclude<E...>, C...>::End() const
    {
        return result.end();
    }

    template <typename... C, typename... E>
    typename View<Exclude<E...>, C...>::Iter View<Exclude<E...>, C...>::begin()
    {
        return Begin();
    }

    template <typename... C, typename... E>
    typename View<Exclude<E...>, C...>::ConstIter View<Exclude<E...>, C...>::begin() const
    {
        return Begin();
    }

    template <typename... C, typename... E>
    typename View<Exclude<E...>, C...>::Iter View<Exclude<E...>, C...>::end()
    {
        return End();
    }

    template <typename... C, typename... E>
    typename View<Exclude<E...>, C...>::ConstIter View<Exclude<E...>, C...>::end() const
    {
        return End();
    }

    template <typename... C, typename... E>
    void View<Exclude<E...>, C...>::Evaluate(ECRegistry& inRegistry)
    {
        std::vector<Internal::CompId> includeCompIds;
        includeCompIds.reserve(sizeof...(C));
        (void) std::initializer_list<int> { ([&]() -> void {
            includeCompIds.emplace_back(typeid(C).hash_code());
        }(), 0)... };

        std::vector<Internal::CompId> excludeCompIds;
        excludeCompIds.reserve(sizeof...(E));
        (void) std::initializer_list<int> { ([&]() -> void {
            excludeCompIds.emplace_back(typeid(E).hash_code());
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
        const Internal::CompId compId = typeid(C).hash_code();
        const Internal::ArchetypeId archetypeId = entities.GetArchetype(inEntity);
        Internal::Archetype& archetype = archetypes.at(archetypeId);

        const Internal::ArchetypeId newArchetypeId = archetypeId + compId;
        entities.SetArchetype(inEntity, newArchetypeId);

        Internal::Archetype* newArchetype;
        if (archetypes.contains(newArchetypeId)) {
            newArchetype = &archetypes.at(newArchetypeId);
            newArchetype->Emplace(inEntity, archetype.Get(inEntity), archetype.GetCompOps());
            archetype.Erase(inEntity);
        } else {
            archetypes.emplace(newArchetypeId, Internal::Archetype(archetype.NewArchetypeAddComp(Internal::CompOp::From<C>())));
            newArchetype = &archetypes.at(newArchetypeId);
            newArchetype->Emplace(inEntity);
        }

        C tempObj(std::forward<Args>(inArgs)...);
        return *static_cast<C*>(newArchetype->EmplaceComp(inEntity, compId, &tempObj));
    }

    template <typename C>
    void ECRegistry::Remove(Entity inEntity)
    {
        Assert(Valid(inEntity) && Has<C>(inEntity));
        const Internal::CompId compId = typeid(C).hash_code();
        const Internal::ArchetypeId archetypeId = entities.GetArchetype(inEntity);
        Internal::Archetype& archetype = archetypes.at(archetypeId);

        const Internal::ArchetypeId newArchetypeId = archetypeId - compId;
        entities.SetArchetype(inEntity, newArchetypeId);

        if (!archetypes.contains(newArchetypeId)) {
            archetypes.emplace(newArchetypeId, Internal::Archetype(archetype.NewArchetypeRemoveComp(Internal::CompOp::From<C>())));
        }
        Internal::Archetype& newArchetype = archetypes.at(newArchetypeId);
        newArchetype.Emplace(inEntity, archetype.Get(inEntity), archetype.GetCompOps());
        archetype.Erase(inEntity);
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
        return ScopedUpdater<C>(*this, inEntity, Get<C>(inEntity));
    }

    template <typename C>
    bool ECRegistry::Has(Entity inEntity) const
    {
        Assert(Valid(inEntity));
        return archetypes
            .at(entities.GetArchetype(inEntity))
            .Contains(typeid(C).hash_code());
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
            .GetComp(inEntity, typeid(C).hash_code());
        return *static_cast<C*>(ptr);
    }

    template <typename C>
    const C& ECRegistry::Get(Entity inEntity) const
    {
        Assert(Valid(inEntity) && Has<C>());
        const void* ptr = archetypes
            .at(entities.GetArchetype(inEntity))
            .GetComp(inEntity, typeid(C).hash_code());
        return *static_cast<const C*>(ptr);
    }

    template <typename C>
    void ECRegistry::NotifyUpdated(Entity inEntity) const
    {
        const auto iter = compEvents.find(typeid(C).hash_code());
        if (iter == compEvents.end()) {
            return;
        }
        iter->second.onUpdated.Broadcast(inEntity);
    }

    template <typename C>
    void ECRegistry::NotifyConstructed(Entity inEntity) const
    {
        const auto iter = compEvents.find(typeid(C).hash_code());
        if (iter == compEvents.end()) {
            return;
        }
        iter->second.onConstructed.Broadcast(inEntity);
    }

    template <typename C>
    void ECRegistry::NotifyRemove(Entity inEntity) const
    {
        const auto iter = compEvents.find(typeid(C).hash_code());
        if (iter == compEvents.end()) {
            return;
        }
        iter->second.onRemove.Broadcast(inEntity);
    }

    template <typename... C, typename... E>
    View<Exclude<E...>, C...> ECRegistry::View(Exclude<E...>)
    {
        return { *this };
    }

    template <typename C>
    ECRegistry::CompEvents& ECRegistry::Events()
    {
        return compEvents[typeid(C).hash_code()];
    }
} // namespace Runtime
