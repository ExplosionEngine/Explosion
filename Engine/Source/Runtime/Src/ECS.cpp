//
// Created by johnk on 2024/10/31.
//

#include <taskflow/taskflow.hpp>

#include <Runtime/ECS.h>

namespace Runtime {
    System::System(ECRegistry& inRegistry, const SystemSetupContext&)
        : registry(inRegistry)
    {
    }

    System::~System() = default;

    void System::Tick(float inDeltaTimeSeconds) {}
}

namespace Runtime::Internal {
    static bool IsGlobalCompClass(GCompClass inClass)
    {
        return inClass->GetMetaBoolOr(MetaPresets::globalComp, false);
    }

    CompRtti::CompRtti(CompClass inClass)
        : clazz(inClass)
        , bound(false)
        , offset(0)
    {
    }

    void CompRtti::Bind(size_t inOffset)
    {
        bound = true;
        offset = inOffset;
    }

    Mirror::Any CompRtti::MoveConstruct(ElemPtr inElem, const Mirror::Any& inOther) const
    {
        auto* compBegin = static_cast<uint8_t*>(inElem) + offset;
        return clazz->InplaceNewDyn(compBegin, { inOther });
    }

    Mirror::Any CompRtti::MoveAssign(ElemPtr inElem, const Mirror::Any& inOther) const
    {
        auto* compBegin = static_cast<uint8_t*>(inElem) + offset;
        auto compRef = clazz->InplaceGetObject(compBegin);
        compRef.MoveAssign(inOther);
        return compRef;
    }

    void CompRtti::Destruct(ElemPtr inElem) const
    {
        auto* compBegin = static_cast<uint8_t*>(inElem) + offset;
        clazz->DestructDyn(clazz->InplaceGetObject(compBegin));
    }

    Mirror::Any CompRtti::Get(ElemPtr inElem) const
    {
        auto* compBegin = static_cast<uint8_t*>(inElem) + offset;
        return clazz->InplaceGetObject(compBegin);
    }

    CompClass CompRtti::Class() const
    {
        return clazz;
    }

    size_t CompRtti::Offset() const
    {
        return offset;
    }

    size_t CompRtti::MemorySize() const
    {
        return clazz->SizeOf();
    }

    Archetype::Archetype(const std::vector<CompRtti>& inRttiVec)
        : id(0)
        , count(0)
        , elemSize(1)
        , rttiVec(inRttiVec)
    {
        rttiMap.reserve(rttiVec.size());
        for (auto i = 0; i < rttiVec.size(); i++) {
            auto& rtti = rttiVec[i];
            const auto clazz = rtti.Class();
            rttiMap.emplace(clazz, i);

            id += clazz->GetTypeInfo()->id;
            rtti.Bind(elemSize);
            elemSize += rtti.MemorySize();
        }
    }

    bool Archetype::Contains(CompClass inClazz) const
    {
        for (const auto& rtti : rttiVec) {
            if (rtti.Class() == inClazz) {
                return true;
            }
        }
        return false;
    }

    bool Archetype::ContainsAll(const std::vector<CompClass>& inClasses) const
    {
        for (const auto& clazz : inClasses) {
            if (!Contains(clazz)) {
                return false;
            }
        }
        return true;
    }

    bool Archetype::NotContainsAny(const std::vector<CompClass>& inClasses) const
    {
        for (const auto& clazz : inClasses) {
            if (Contains(clazz)) {
                return false;
            }
        }
        return true;
    }

    ElemPtr Archetype::EmplaceElem(Entity inEntity)
    {
        ElemPtr result = AllocateNewElemBack();
        auto backElem = count - 1;
        entityMap.emplace(inEntity, backElem);
        elemMap.emplace(backElem, inEntity);
        return result;
    }

    ElemPtr Archetype::EmplaceElem(Entity inEntity, ElemPtr inSrcElem, const std::vector<CompRtti>& inSrcRttiVec)
    {
        ElemPtr newElem = EmplaceElem(inEntity);
        for (const auto& srcRtti : inSrcRttiVec) {
            const auto* newRtti = FindCompRtti(srcRtti.Class());
            if (newRtti == nullptr) {
                continue;
            }
            newRtti->MoveConstruct(newElem, srcRtti.Get(inSrcElem));
        }
        return newElem;
    }

    Mirror::Any Archetype::EmplaceComp(Entity inEntity, CompClass inCompClass, const Mirror::Any& inCompRef) // NOLINT
    {
        ElemPtr elem = ElemAt(entityMap.at(inEntity));
        return GetCompRtti(inCompClass).MoveConstruct(elem, inCompRef);
    }

    void Archetype::EraseElem(Entity inEntity)
    {
        const auto elemIndex = entityMap.at(inEntity);
        ElemPtr elem = ElemAt(elemIndex);
        const auto lastElemIndex = count - 1;
        const auto entityToLastElem = elemMap.at(lastElemIndex);
        ElemPtr lastElem = ElemAt(lastElemIndex);
        for (const auto& rtti : rttiVec) {
            rtti.MoveAssign(elem, rtti.Get(lastElem));
        }
        entityMap.at(entityToLastElem) = elemIndex;
        entityMap.erase(inEntity);
        elemMap.at(elemIndex) = entityToLastElem;
        elemMap.erase(lastElemIndex);
        count--;
    }

    ElemPtr Archetype::GetElem(Entity inEntity) const
    {
        return ElemAt(entityMap.at(inEntity));
    }

    Mirror::Any Archetype::GetComp(Entity inEntity, CompClass inCompClass)
    {
        ElemPtr element = GetElem(inEntity);
        return GetCompRtti(inCompClass).Get(element);
    }

    Mirror::Any Archetype::GetComp(Entity inEntity, CompClass inCompClass) const
    {
        ElemPtr element = GetElem(inEntity);
        return GetCompRtti(inCompClass).Get(element).ConstRef();
    }

    size_t Archetype::Count() const
    {
        return count;
    }

    const std::vector<CompRtti>& Archetype::GetRttiVec() const
    {
        return rttiVec;
    }

    ArchetypeId Archetype::Id() const
    {
        return id;
    }

    std::vector<CompRtti> Archetype::NewRttiVecByAdd(const CompRtti& inRtti) const
    {
        auto result = rttiVec;
        result.emplace_back(inRtti);
        return result;
    }

    std::vector<CompRtti> Archetype::NewRttiVecByRemove(const CompRtti& inRtti) const
    {
        auto result = rttiVec;
        const auto iter = std::ranges::find_if(result, [&](const CompRtti& rtti) -> bool { return rtti.Class() == inRtti.Class(); });
        Assert(iter != result.end());
        result.erase(iter);
        return result;
    }

    const CompRtti* Archetype::FindCompRtti(CompClass clazz) const
    {
        const auto iter = rttiMap.find(clazz);
        return iter != rttiMap.end() ? &rttiVec[iter->second] : nullptr;
    }

    const CompRtti& Archetype::GetCompRtti(CompClass clazz) const
    {
        Assert(rttiMap.contains(clazz));
        return rttiVec[rttiMap.at(clazz)];
    }

    ElemPtr Archetype::ElemAt(std::vector<uint8_t>& inMemory, size_t inIndex) const // NOLINT
    {
        return inMemory.data() + (inIndex * elemSize);
    }

    ElemPtr Archetype::ElemAt(size_t inIndex) const
    {
        return ElemAt(const_cast<std::vector<uint8_t>&>(memory), inIndex);
    }

    size_t Archetype::Capacity() const
    {
        return memory.size() / elemSize;
    }

    void Archetype::Reserve(float inRatio)
    {
        Assert(inRatio > 1.0f);
        const size_t newCapacity = static_cast<size_t>(std::ceil(static_cast<float>(std::max(Capacity(), static_cast<size_t>(1))) * inRatio));
        std::vector<uint8_t> newMemory(newCapacity * elemSize);

        for (auto i = 0; i < count; i++) {
            for (const auto& rtti : rttiVec) {
                void* dstElem = ElemAt(newMemory, i);
                void* srcElem = ElemAt(i);
                rtti.MoveConstruct(dstElem, rtti.Get(srcElem));
                rtti.Destruct(srcElem);
            }
        }
        memory = std::move(newMemory);
    }

    void* Archetype::AllocateNewElemBack()
    {
        if (Count() == Capacity()) {
            Reserve();
        }
        count++;
        return ElemAt(count - 1);
    }

    EntityPool::EntityPool()
        : counter(1)
    {
    }

    size_t EntityPool::Count() const
    {
        return allocated.size();
    }

    bool EntityPool::Valid(Entity inEntity) const
    {
        return allocated.contains(inEntity);
    }

    Entity EntityPool::Allocate()
    {
        Entity result = entityNull;
        if (!free.empty()) {
            result = *free.begin();
            free.erase(result);
        } else {
            result = counter++;
        }
        allocated.emplace(result);
        SetArchetype(result, 0);
        return result;
    }

    void EntityPool::Allocate(Entity inEntity)
    {
        if (inEntity < counter) {
            Assert(free.contains(inEntity));
            free.erase(inEntity);
        } else {
            for (uint32_t i = counter; i < inEntity; i++) {
                free.emplace(i);
            }
            counter = inEntity + 1;
        }
        allocated.emplace(inEntity);
        SetArchetype(inEntity, 0);
    }

    void EntityPool::Free(Entity inEntity)
    {
        Assert(Valid(inEntity));
        allocated.erase(inEntity);
        free.emplace(inEntity);
        archetypeMap.erase(inEntity);
    }

    void EntityPool::Clear()
    {
        counter = 1;
        free.clear();
        allocated.clear();
        archetypeMap.clear();
    }

    void EntityPool::Each(const EntityTraverseFunc& inFunc) const
    {
        for (const auto& entity : allocated) {
            inFunc(entity);
        }
    }

    void EntityPool::SetArchetype(Entity inEntity, ArchetypeId inArchetypeId)
    {
        archetypeMap[inEntity] = inArchetypeId;
    }

    ArchetypeId EntityPool::GetArchetype(Entity inEntity) const
    {
        return archetypeMap.at(inEntity);
    }

    EntityPool::ConstIter EntityPool::Begin() const
    {
        return allocated.begin();
    }

    EntityPool::ConstIter EntityPool::End() const
    {
        return allocated.end();
    }

    SystemFactory::SystemFactory(SystemClass inClass)
        : clazz(inClass)
    {
        BuildArgumentLists();
    }

    Common::UniquePtr<System> SystemFactory::Build(ECRegistry& inRegistry, const SystemSetupContext& inSetupContext) const
    {
        const Mirror::Any system = clazz->New(inRegistry, inSetupContext);
        const Mirror::Any systemRef = system.Deref();
        for (const auto& [name, argument] : arguments) {
            clazz->GetMemberVariable(name).SetDyn(systemRef, argument.ConstRef());
        }
        return system.As<System*>();
    }

    std::unordered_map<std::string, Mirror::Any> SystemFactory::GetArguments()
    {
        std::unordered_map<std::string, Mirror::Any> result;
        for (auto& [name, argument] : arguments) {
            result.emplace(name, argument.Ref());
        }
        return result;
    }

    const std::unordered_map<std::string, Mirror::Any>& SystemFactory::GetArguments() const
    {
        return arguments;
    }

    SystemClass SystemFactory::GetClass() const
    {
        return clazz;
    }

    void SystemFactory::BuildArgumentLists()
    {
        const auto& memberVariables = clazz->GetMemberVariables();
        arguments.reserve(memberVariables.size());
        for (const auto& [id, member] : memberVariables) {
            arguments.emplace(id.name, member.GetDyn(clazz->GetDefaultObject()));
        }
    }
} // namespace Runtime::Internal

namespace Runtime {
    ScopedUpdaterDyn::ScopedUpdaterDyn(ECRegistry& inRegistry, CompClass inClass, Entity inEntity, const Mirror::Any& inCompRef)
        : registry(inRegistry)
        , clazz(inClass)
        , entity(inEntity)
        , compRef(inCompRef)
    {
    }

    ScopedUpdaterDyn::~ScopedUpdaterDyn()
    {
        registry.NotifyUpdatedDyn(clazz, entity);
    }

    GScopedUpdaterDyn::GScopedUpdaterDyn(ECRegistry& inRegistry, GCompClass inClass, const Mirror::Any& inGlobalCompRef)
        : registry(inRegistry)
        , clazz(inClass)
        , globalCompRef(inGlobalCompRef)
    {
    }

    GScopedUpdaterDyn::~GScopedUpdaterDyn()
    {
        registry.GNotifyUpdatedDyn(clazz);
    }

    RuntimeFilter::RuntimeFilter() = default;

    RuntimeFilter& RuntimeFilter::IncludeDyn(CompClass inClass)
    {
        Assert(!includes.contains(inClass));
        includes.emplace(inClass);
        return *this;
    }

    RuntimeFilter& RuntimeFilter::ExcludeDyn(CompClass inClass)
    {
        Assert(!excludes.contains(inClass));
        excludes.emplace(inClass);
        return *this;
    }

    Observer::Observer(ECRegistry& inRegistry)
        : registry(inRegistry)
    {
    }

    Observer::~Observer()
    {
        UnbindAll();
    }

    Observer& Observer::ObConstructedDyn(CompClass inClass)
    {
        return OnEvent(registry.EventsDyn(inClass).onConstructed);
    }

    Observer& Observer::ObUpdatedDyn(CompClass inClass)
    {
        return OnEvent(registry.EventsDyn(inClass).onUpdated);
    }

    Observer& Observer::ObRemoved(CompClass inClass)
    {
        return OnEvent(registry.EventsDyn(inClass).onRemove);
    }

    size_t Observer::Count() const
    {
        return entities.size();
    }

    void Observer::Each(const EntityTraverseFunc& inFunc) const
    {
        for (const auto& entity : entities) {
            inFunc(entity);
        }
    }

    void Observer::EachThenClear(const EntityTraverseFunc& inFunc)
    {
        Each(inFunc);
        Clear();
    }

    void Observer::Clear()
    {
        entities.clear();
    }

    const std::vector<Entity>& Observer::All() const
    {
        return entities;
    }

    std::vector<Entity> Observer::Pop()
    {
        auto result = entities;
        Clear();
        return result;
    }

    void Observer::UnbindAll()
    {
        for (auto& deleter : receiverHandles | std::views::values) {
            deleter();
        }
        receiverHandles.clear();
    }

    Observer::ConstIter Observer::Begin() const
    {
        return entities.begin();
    }

    Observer::ConstIter Observer::End() const
    {
        return entities.end();
    }

    Observer::ConstIter Observer::begin() const
    {
        return Begin();
    }

    Observer::ConstIter Observer::end() const
    {
        return End();
    }

    void Observer::RecordEntity(ECRegistry& inRegistry, Entity inEntity)
    {
        entities.emplace_back(inEntity);
    }

    Observer& Observer::OnEvent(Common::Delegate<ECRegistry&, Entity>& inEvent)
    {
        const auto handle = inEvent.BindMember<&Observer::RecordEntity>(*this);
        receiverHandles.emplace_back(
            handle,
            [&inEvent, handle]() -> void {
                inEvent.Unbind(handle);
            });
        return *this;
    }

    EventsObserverDyn::EventsObserverDyn(ECRegistry& inRegistry, CompClass inClass)
        : constructedObserver(inRegistry.Observer())
        , updatedObserver(inRegistry.Observer())
        , removedObserver(inRegistry.Observer())
    {
        constructedObserver.ObConstructedDyn(inClass);
        updatedObserver.ObUpdatedDyn(inClass);
        removedObserver.ObRemoved(inClass);
    }

    EventsObserverDyn::~EventsObserverDyn() = default;

    size_t EventsObserverDyn::ConstructedCount() const
    {
        return constructedObserver.Count();
    }

    size_t EventsObserverDyn::UpdatedCount() const
    {
        return updatedObserver.Count();
    }

    size_t EventsObserverDyn::RemovedCount() const
    {
        return removedObserver.Count();
    }

    void EventsObserverDyn::EachConstructed(const EntityTraverseFunc& inFunc) const
    {
        constructedObserver.Each(inFunc);
    }

    void EventsObserverDyn::EachUpdated(const EntityTraverseFunc& inFunc) const
    {
        updatedObserver.Each(inFunc);
    }

    void EventsObserverDyn::EachRemoved(const EntityTraverseFunc& inFunc) const
    {
        removedObserver.Each(inFunc);
    }

    void EventsObserverDyn::ClearConstructed()
    {
        constructedObserver.Clear();
    }

    void EventsObserverDyn::ClearUpdated()
    {
        updatedObserver.Clear();
    }

    void EventsObserverDyn::ClearRemoved()
    {
        removedObserver.Clear();
    }

    void EventsObserverDyn::Clear()
    {
        ClearConstructed();
        ClearUpdated();
        ClearRemoved();
    }

    const auto& EventsObserverDyn::Constructed() const
    {
        return constructedObserver;
    }

    const auto& EventsObserverDyn::Updated() const
    {
        return updatedObserver;
    }

    const auto& EventsObserverDyn::Removed() const
    {
        return removedObserver;
    }

    ECRegistry::ECRegistry()
    {
        archetypes.emplace(0, Internal::Archetype({}));
    }

    ECRegistry::~ECRegistry()
    {
        CheckEventsUnbound();
    }

    ECRegistry::ECRegistry(const ECRegistry& inOther)
        : entities(inOther.entities)
        , globalComps(inOther.globalComps)
        , archetypes(inOther.archetypes)
    {
    }

    ECRegistry::ECRegistry(ECRegistry&& inOther) noexcept
        : entities(std::move(inOther.entities))
        , globalComps(std::move(inOther.globalComps))
        , archetypes(std::move(inOther.archetypes))
    {
    }

    ECRegistry& ECRegistry::operator=(const ECRegistry& inOther)
    {
        entities = inOther.entities;
        globalComps = inOther.globalComps;
        archetypes = inOther.archetypes;
        return *this;
    }

    ECRegistry& ECRegistry::operator=(ECRegistry&& inOther) noexcept
    {
        entities = std::move(inOther.entities);
        globalComps = std::move(inOther.globalComps);
        archetypes = std::move(inOther.archetypes);
        return *this;
    }

    Entity ECRegistry::Create()
    {
        const Entity result = entities.Allocate();
        archetypes.at(entities.GetArchetype(result)).EmplaceElem(result);
        return result;
    }

    void ECRegistry::Create(Entity inEntity)
    {
        entities.Allocate(inEntity);
        archetypes.at(entities.GetArchetype(inEntity)).EmplaceElem(inEntity);
    }

    void ECRegistry::Destroy(Entity inEntity)
    {
        Assert(Valid(inEntity));
        archetypes.at(entities.GetArchetype(inEntity)).EraseElem(inEntity);
        entities.Free(inEntity);
    }

    bool ECRegistry::Valid(Entity inEntity) const
    {
        return entities.Valid(inEntity);
    }

    size_t ECRegistry::Count() const
    {
        return entities.Count();
    }

    void ECRegistry::Clear()
    {
        entities.Clear();
        globalComps.clear();
        archetypes.clear();
        archetypes.emplace(0, Internal::Archetype({}));
    }

    void ECRegistry::Each(const EntityTraverseFunc& inFunc) const
    {
        entities.Each(inFunc);
    }

    ECRegistry::ConstIter ECRegistry::Begin() const
    {
        return entities.Begin();
    }

    ECRegistry::ConstIter ECRegistry::End() const
    {
        return entities.End();
    }

    ECRegistry::ConstIter ECRegistry::begin() const
    {
        return Begin();
    }

    ECRegistry::ConstIter ECRegistry::end() const
    {
        return End();
    }

    void ECRegistry::CompEach(Entity inEntity, const CompTraverseFunc& inFunc) const
    {
        const Internal::ArchetypeId archetypeId = entities.GetArchetype(inEntity);
        const Internal::Archetype& archetype = archetypes.at(archetypeId);
        for (const auto& compRtti : archetype.GetRttiVec()) {
            inFunc(compRtti.Class());
        }
    }

    size_t ECRegistry::CompCount(Entity inEntity) const
    {
        const Internal::ArchetypeId archetypeId = entities.GetArchetype(inEntity);
        const Internal::Archetype& archetype = archetypes.at(archetypeId);
        return archetype.GetRttiVec().size();
    }

    Runtime::RuntimeView ECRegistry::RuntimeView(const RuntimeFilter& inFilter)
    {
        return Runtime::RuntimeView { *this, inFilter };
    }

    Runtime::ConstRuntimeView ECRegistry::RuntimeView(const RuntimeFilter& inFilter) const
    {
        return Runtime::ConstRuntimeView { *this, inFilter };
    }

    Runtime::ConstRuntimeView ECRegistry::ConstRuntimeView(const RuntimeFilter& inFilter) const
    {
        return Runtime::ConstRuntimeView { *this, inFilter };
    }

    EventsObserverDyn ECRegistry::EventsObserverDyn(CompClass inClass)
    {
        return Runtime::EventsObserverDyn { *this, inClass };
    }

    void ECRegistry::NotifyUpdatedDyn(CompClass inClass, Entity inEntity)
    {
        const auto iter = compEvents.find(inClass);
        if (iter == compEvents.end()) {
            return;
        }
        iter->second.onUpdated.Broadcast(*this, inEntity);
    }

    void ECRegistry::NotifyConstructedDyn(CompClass inClass, Entity inEntity)
    {
        const auto iter = compEvents.find(inClass);
        if (iter == compEvents.end()) {
            return;
        }
        iter->second.onConstructed.Broadcast(*this, inEntity);
    }

    void ECRegistry::NotifyRemoveDyn(CompClass inClass, Entity inEntity)
    {
        const auto iter = compEvents.find(inClass);
        if (iter == compEvents.end()) {
            return;
        }
        iter->second.onRemove.Broadcast(*this, inEntity);
    }

    Observer ECRegistry::Observer()
    {
        return Runtime::Observer { *this };
    }

    void ECRegistry::Save(ECArchive& outArchive) const
    {
        outArchive = {};
        outArchive.entities.reserve(Count());
        Each([&](Entity entity) -> void {
            outArchive.entities.emplace(entity, EntityArchive {});
            auto& comps = outArchive.entities.at(entity).comps;
            comps.reserve(CompCount(entity));

            CompEach(entity, [&](CompClass clazz) -> void {
                if (clazz->IsTransient()) {
                    return;
                }
                comps.emplace(clazz, std::vector<uint8_t> {});
                Common::MemorySerializeStream stream(comps.at(clazz));
                GetDyn(clazz, entity).Serialize(stream);
            });
        });

        auto& gComps = outArchive.globalComps;
        gComps.reserve(GCompCount());
        GCompEach([&](GCompClass clazz) -> void {
            if (clazz->IsTransient()) {
                return;
            }
            gComps.emplace(clazz, std::vector<uint8_t> {});
            Common::MemorySerializeStream stream(gComps.at(clazz));
            GGetDyn(clazz).Serialize(stream);
        });
    }

    void ECRegistry::Load(const ECArchive& inArchive)
    {
        Clear();

        for (const auto& [entity, entityArchive] : inArchive.entities) {
            Create(entity);
            for (const auto& [compClass, compData] : entityArchive.comps) {
                if (compClass->IsTransient()) {
                    continue;
                }
                Assert(compClass->HasDefaultConstructor());
                Mirror::Any compRef = EmplaceDyn(compClass, entity, {});
                Common::MemoryDeserializeStream stream(compData);
                compRef.Deserialize(stream);
            }
        }

        for (const auto& [gCompClass, gCompData] : inArchive.globalComps) {
            if (gCompClass->IsTransient()) {
                continue;
            }
            Assert(gCompClass->HasDefaultConstructor());
            Mirror::Any gCompRef = GEmplaceDyn(gCompClass, {});
            Common::MemoryDeserializeStream stream(gCompData);
            gCompRef.Deserialize(stream);
        }
    }

    void ECRegistry::CheckEventsUnbound() const
    {
        for (const auto& events : compEvents | std::views::values) {
            Assert(events.onConstructed.Count() == 0);
            Assert(events.onUpdated.Count() == 0);
            Assert(events.onRemove.Count() == 0);
        }

        for (const auto& events : globalCompEvents | std::views::values) {
            Assert(events.onConstructed.Count() == 0);
            Assert(events.onUpdated.Count() == 0);
            Assert(events.onRemove.Count() == 0);
        }
    }

    Mirror::Any ECRegistry::EmplaceDyn(CompClass inClass, Entity inEntity, const Mirror::ArgumentList& inArgs)
    {
        Assert(Valid(inEntity));
        const Internal::ArchetypeId archetypeId = entities.GetArchetype(inEntity);
        Internal::Archetype& archetype = archetypes.at(archetypeId);

        const Internal::ArchetypeId newArchetypeId = archetypeId + inClass->GetTypeInfo()->id;
        entities.SetArchetype(inEntity, newArchetypeId);

        if (!archetypes.contains(newArchetypeId)) {
            archetypes.emplace(newArchetypeId, Internal::Archetype(archetype.NewRttiVecByAdd(Internal::CompRtti(inClass))));
        }
        Internal::Archetype& newArchetype = archetypes.at(newArchetypeId);
        newArchetype.EmplaceElem(inEntity, archetype.GetElem(inEntity), archetype.GetRttiVec());
        archetype.EraseElem(inEntity);

        Mirror::Any tempObj = inClass->ConstructDyn(inArgs);
        Mirror::Any compRef = newArchetype.EmplaceComp(inEntity, inClass, tempObj.Ref());
        NotifyConstructedDyn(inClass, inEntity);
        return compRef;
    }

    void ECRegistry::RemoveDyn(CompClass inClass, Entity inEntity)
    {
        Assert(Valid(inEntity) && HasDyn(inClass, inEntity));
        const Internal::ArchetypeId archetypeId = entities.GetArchetype(inEntity);
        Internal::Archetype& archetype = archetypes.at(archetypeId);

        const Internal::ArchetypeId newArchetypeId = archetypeId - inClass->GetTypeInfo()->id;
        entities.SetArchetype(inEntity, newArchetypeId);

        if (!archetypes.contains(newArchetypeId)) {
            archetypes.emplace(newArchetypeId, Internal::Archetype(archetype.NewRttiVecByRemove(Internal::CompRtti(inClass))));
        }
        NotifyRemoveDyn(inClass, inEntity);
        Internal::Archetype& newArchetype = archetypes.at(newArchetypeId);
        newArchetype.EmplaceElem(inEntity, archetype.GetElem(inEntity), archetype.GetRttiVec());
        archetype.EraseElem(inEntity);
    }

    void ECRegistry::UpdateDyn(CompClass inClass, Entity inEntity, const DynUpdateFunc& inFunc)
    {
        Assert(Valid(inEntity) && HasDyn(inClass, inEntity));
        inFunc(GetDyn(inClass, inEntity));
        NotifyUpdatedDyn(inClass, inEntity);
    }

    ScopedUpdaterDyn ECRegistry::UpdateDyn(CompClass inClass, Entity inEntity)
    {
        Assert(Valid(inEntity) && HasDyn(inClass, inEntity));
        return { *this, inClass, inEntity, GetDyn(inClass, inEntity) };
    }

    bool ECRegistry::HasDyn(CompClass inClass, Entity inEntity) const
    {
        Assert(Valid(inEntity));
        return archetypes
            .at(entities.GetArchetype(inEntity))
            .Contains(inClass);
    }

    Mirror::Any ECRegistry::FindDyn(CompClass inClass, Entity inEntity)
    {
        return HasDyn(inClass, inEntity) ? GetDyn(inClass, inEntity) : Mirror::Any();
    }

    Mirror::Any ECRegistry::FindDyn(CompClass inClass, Entity inEntity) const
    {
        return HasDyn(inClass, inEntity) ? GetDyn(inClass, inEntity) : Mirror::Any();
    }

    Mirror::Any ECRegistry::GetDyn(CompClass inClass, Entity inEntity)
    {
        Assert(Valid(inEntity) && HasDyn(inClass, inEntity));
        Mirror::Any compRef = archetypes
            .at(entities.GetArchetype(inEntity))
            .GetComp(inEntity, inClass);
        return compRef;
    }

    Mirror::Any ECRegistry::GetDyn(CompClass inClass, Entity inEntity) const
    {
        Assert(Valid(inEntity) && HasDyn(inClass, inEntity));
        Mirror::Any compRef = archetypes
            .at(entities.GetArchetype(inEntity))
            .GetComp(inEntity, inClass);
        return compRef.ConstRef();
    }

    ECRegistry::CompEvents& ECRegistry::EventsDyn(CompClass inClass)
    {
        return compEvents[inClass];
    }

    void ECRegistry::GNotifyUpdatedDyn(GCompClass inClass)
    {
        const auto iter = globalCompEvents.find(inClass);
        if (iter == globalCompEvents.end()) {
            return;
        }
        iter->second.onUpdated.Broadcast(*this);
    }

    void ECRegistry::GNotifyConstructedDyn(GCompClass inClass)
    {
        const auto iter = globalCompEvents.find(inClass);
        if (iter == globalCompEvents.end()) {
            return;
        }
        iter->second.onConstructed.Broadcast(*this);
    }

    void ECRegistry::GNotifyRemoveDyn(CompClass inClass)
    {
        const auto iter = globalCompEvents.find(inClass);
        if (iter == globalCompEvents.end()) {
            return;
        }
        iter->second.onRemove.Broadcast(*this);
    }

    Mirror::Any ECRegistry::GEmplaceDyn(GCompClass inClass, const Mirror::ArgumentList& inArgs)
    {
        Assert(Internal::IsGlobalCompClass(inClass));
        Assert(!GHasDyn(inClass));
        globalComps.emplace(inClass, inClass->ConstructDyn(inArgs));
        GNotifyConstructedDyn(inClass);
        return GGetDyn(inClass);
    }

    void ECRegistry::GRemoveDyn(GCompClass inClass)
    {
        Assert(Internal::IsGlobalCompClass(inClass));
        Assert(GHasDyn(inClass));
        GNotifyRemoveDyn(inClass);
        globalComps.erase(inClass);
    }

    void ECRegistry::GUpdateDyn(GCompClass inClass, const DynUpdateFunc& inFunc)
    {
        Assert(Internal::IsGlobalCompClass(inClass));
        Assert(GHasDyn(inClass));
        inFunc(GGetDyn(inClass));
        GNotifyUpdatedDyn(inClass);
    }

    GScopedUpdaterDyn ECRegistry::GUpdateDyn(GCompClass inClass)
    {
        Assert(Internal::IsGlobalCompClass(inClass));
        Assert(GHasDyn(inClass));
        return { *this, inClass, GGetDyn(inClass) };
    }

    bool ECRegistry::GHasDyn(GCompClass inClass) const
    {
        Assert(Internal::IsGlobalCompClass(inClass));
        return globalComps.contains(inClass);
    }

    Mirror::Any ECRegistry::GFindDyn(GCompClass inClass)
    {
        Assert(Internal::IsGlobalCompClass(inClass));
        return GHasDyn(inClass) ? GGetDyn(inClass) : Mirror::Any();
    }

    Mirror::Any ECRegistry::GFindDyn(GCompClass inClass) const
    {
        Assert(Internal::IsGlobalCompClass(inClass));
        return GHasDyn(inClass) ? GGetDyn(inClass) : Mirror::Any();
    }

    Mirror::Any ECRegistry::GGetDyn(GCompClass inClass)
    {
        Assert(Internal::IsGlobalCompClass(inClass));
        Assert(GHasDyn(inClass));
        return globalComps.at(inClass).Ref();
    }

    Mirror::Any ECRegistry::GGetDyn(GCompClass inClass) const
    {
        Assert(Internal::IsGlobalCompClass(inClass));
        Assert(GHasDyn(inClass));
        return globalComps.at(inClass).ConstRef();
    }

    ECRegistry::GCompEvents& ECRegistry::GEventsDyn(GCompClass inClass)
    {
        Assert(Internal::IsGlobalCompClass(inClass));
        return globalCompEvents[inClass];
    }

    void ECRegistry::GCompEach(const GCompTraverseFunc& inFunc) const
    {
        for (const auto& clazz : globalComps | std::views::keys) {
            inFunc(clazz);
        }
    }

    size_t ECRegistry::GCompCount() const
    {
        return globalComps.size();
    }

    SystemGroup::SystemGroup(std::string inName, SystemExecuteStrategy inStrategy)
        : name(std::move(inName))
        , strategy(inStrategy)
    {
    }

    Internal::SystemFactory& SystemGroup::EmplaceSystemDyn(SystemClass inClass)
    {
        Assert(!HasSystemDyn(inClass));
        return systems.emplace_back(inClass);
    }

    void SystemGroup::RemoveSystemDyn(SystemClass inClass)
    {
        const auto iter = FindSystem(inClass);
        Assert(iter != systems.end());
        systems.erase(iter);
    }

    bool SystemGroup::HasSystemDyn(SystemClass inClass) const
    {
        const auto iter = FindSystem(inClass);
        return iter != systems.end();
    }

    Internal::SystemFactory& SystemGroup::GetSystemDyn(SystemClass inClass)
    {
        const auto iter = FindSystem(inClass);
        Assert(iter != systems.end());
        return *iter;
    }

    const Internal::SystemFactory& SystemGroup::GetSystemDyn(SystemClass inClass) const
    {
        const auto iter = FindSystem(inClass);
        Assert(iter != systems.end());
        return *iter;
    }

    Internal::SystemFactory& SystemGroup::MoveSystemToDyn(SystemClass inSrcClass, SystemClass inDstClass)
    {
        const auto srcIter = FindSystem(inSrcClass);
        Assert(srcIter != systems.end());
        const auto tempGroup = std::move(*srcIter);
        systems.erase(srcIter);

        const auto dstIter = FindSystem(inDstClass);
        Assert(dstIter != systems.end());
        return *systems.emplace(dstIter, std::move(tempGroup));
    }

    const std::vector<Internal::SystemFactory>& SystemGroup::GetSystems()
    {
        return systems;
    }

    const std::vector<Internal::SystemFactory>& SystemGroup::GetSystems() const
    {
        return systems;
    }

    const std::string& SystemGroup::GetName() const
    {
        return name;
    }

    SystemExecuteStrategy SystemGroup::GetStrategy() const
    {
        return strategy;
    }

    std::vector<Internal::SystemFactory>::iterator SystemGroup::FindSystem(SystemClass inClass)
    {
        return std::ranges::find_if(systems, [inClass](const Internal::SystemFactory& inFactory) -> bool {
            return inFactory.GetClass() == inClass;
        });
    }

    std::vector<Internal::SystemFactory>::const_iterator SystemGroup::FindSystem(SystemClass inClass) const
    {
        return std::ranges::find_if(systems, [inClass](const Internal::SystemFactory& inFactory) -> bool {
            return inFactory.GetClass() == inClass;
        });
    }

    SystemGraph::SystemGraph() = default;

    SystemGroup& SystemGraph::AddGroup(const std::string& inName, SystemExecuteStrategy inStrategy)
    {
        Assert(!HasGroup(inName));
        return systemGroups.emplace_back(inName, inStrategy);
    }

    void SystemGraph::RemoveGroup(const std::string& inName)
    {
        const auto iter = FindGroup(inName);
        Assert(iter != systemGroups.end());
        systemGroups.erase(iter);
    }

    bool SystemGraph::HasGroup(const std::string& inName) const
    {
        const auto iter = FindGroup(inName);
        return iter != systemGroups.end();
    }

    SystemGroup& SystemGraph::GetGroup(const std::string& inName)
    {
        const auto iter = FindGroup(inName);
        Assert(iter != systemGroups.end());
        return *iter;
    }

    const SystemGroup& SystemGraph::GetGroup(const std::string& inName) const
    {
        const auto iter = FindGroup(inName);
        Assert(iter != systemGroups.end());
        return *iter;
    }

    const std::vector<SystemGroup>& SystemGraph::GetGroups() const
    {
        return systemGroups;
    }

    SystemGroup& SystemGraph::MoveGroupTo(const std::string& inSrcName, const std::string& inDstName)
    {
        const auto srcIter = FindGroup(inSrcName);
        Assert(srcIter != systemGroups.end());
        const auto tempGroup = std::move(*srcIter);
        systemGroups.erase(srcIter);

        const auto dstIter = FindGroup(inDstName);
        Assert(dstIter != systemGroups.end());
        return *systemGroups.emplace(dstIter, std::move(tempGroup));
    }

    std::vector<SystemGroup>::iterator SystemGraph::FindGroup(const std::string& inName)
    {
        return std::ranges::find_if(systemGroups, [&](const SystemGroup& group) -> bool {
            return group.GetName() == inName;
        });
    }

    std::vector<SystemGroup>::const_iterator SystemGraph::FindGroup(const std::string& inName) const
    {
        return std::ranges::find_if(systemGroups, [&](const SystemGroup& group) -> bool {
            return group.GetName() == inName;
        });
    }

    SystemPipeline::SystemPipeline(const SystemGraph& inGraph)
    {
        const auto& systemGroups = inGraph.GetGroups();
        systemGraph.reserve(systemGroups.size());

        for (const auto& group : systemGroups) {
            auto& [systemContexts, strategy] = systemGraph.emplace_back();
            const auto& factories = group.GetSystems();

            strategy = group.GetStrategy();
            systemContexts.reserve(factories.size());
            for (const auto& factory : group.GetSystems()) {
                systemContexts.emplace_back(factory, nullptr);
            }
        }
    }

    void SystemPipeline::ParallelPerformAction(const ActionFunc& inActionFunc)
    {
        tf::Taskflow taskFlow;
        auto lastBarrier = taskFlow.emplace([]() -> void {});

        for (auto& groupContext : systemGraph) {
            if (groupContext.strategy == SystemExecuteStrategy::sequential) {
                tf::Task lastTask = lastBarrier;
                for (auto& systemContext : groupContext.systems) {
                    auto task = taskFlow.emplace([&]() -> void {
                        inActionFunc(systemContext);
                    });
                    task.succeed(lastTask);
                    lastTask = task;
                }
                lastBarrier = lastTask;
            } else if (groupContext.strategy == SystemExecuteStrategy::concurrent) {
                std::vector<tf::Task> tasks;
                tasks.reserve(groupContext.systems.size());

                for (auto& systemContext : groupContext.systems) {
                    tasks.emplace_back(taskFlow.emplace([&]() -> void {
                        inActionFunc(systemContext);
                    }));
                    tasks.back().succeed(lastBarrier);
                }

                auto barrier = taskFlow.emplace([]() -> void {});
                for (const auto& task : tasks) {
                    barrier.succeed(task);
                }
                lastBarrier = barrier;
            } else {
                QuickFail();
            }
        }

        tf::Executor executor;
        executor
            .run(taskFlow)
            .wait();
    }

    SystemSetupContext::SystemSetupContext()
        : playType(PlayType::max)
        , client(nullptr)
    {
    }

    SystemGraphExecutor::SystemGraphExecutor(ECRegistry& inEcRegistry, const SystemGraph& inSystemGraph, const SystemSetupContext& inSetupContext)
        : ecRegistry(inEcRegistry)
        , systemGraph(inSystemGraph)
        , pipeline(inSystemGraph)
    {
        pipeline.ParallelPerformAction([&](SystemPipeline::SystemContext& context) -> void {
            context.instance = context.factory.Build(inEcRegistry, inSetupContext);
        });
    }

    SystemGraphExecutor::~SystemGraphExecutor()
    {
        pipeline.ParallelPerformAction([](SystemPipeline::SystemContext& context) -> void {
            context.instance = nullptr;
        });
        ecRegistry.CheckEventsUnbound();
    }

    void SystemGraphExecutor::Tick(float inDeltaTimeSeconds)
    {
        pipeline.ParallelPerformAction([&](const SystemPipeline::SystemContext& context) -> void {
            context.instance->Tick(inDeltaTimeSeconds);
        });
    }
} // namespace Runtime
