//
// Created by johnk on 2024/10/31.
//

#include <Runtime/ECS.h>

namespace Runtime::Internal {
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

    size_t CompRtti::Size() const
    {
        return clazz->SizeOf();
    }

    Archetype::Archetype(const std::vector<CompRtti>& inRttiVec)
        : id(0)
        , size(0)
        , elemSize(0)
        , rttiVec(inRttiVec)
    {
        rttiMap.reserve(rttiVec.size());
        for (auto i = 0; i < rttiVec.size(); i++) {
            auto& rtti = rttiVec[i];
            const auto clazz = rtti.Class();
            rttiMap.emplace(clazz, i);

            id += clazz->GetTypeInfo()->id;
            rtti.Bind(elemSize);
            elemSize += rtti.Size();
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
        for (const auto& rtti : rttiVec) {
            if (!Contains(rtti.Class())) {
                return false;
            }
        }
        return true;
    }

    bool Archetype::NotContainsAny(const std::vector<CompClass>& inCompClasses) const
    {
        for (const auto& rtti : rttiVec) {
            if (Contains(rtti.Class())) {
                return false;
            }
        }
        return true;
    }

    ElemPtr Archetype::EmplaceElem(Entity inEntity)
    {
        ElemPtr result = AllocateNewElemBack();
        auto backElem = size - 1;
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

    Mirror::Any Archetype::EmplaceComp(Entity inEntity, CompClass inCompClass, const Mirror::Argument& inCompRef) // NOLINT
    {
        ElemPtr elem = ElemAt(entityMap.at(inEntity));
        return GetCompRtti(inCompClass).MoveConstruct(elem, inCompRef);
    }

    void Archetype::EraseElem(Entity inEntity)
    {
        const auto elemIndex = entityMap.at(inEntity);
        ElemPtr elem = ElemAt(elemIndex);
        const auto lastElemIndex = Size() - 1;
        const auto entityToLastElem = elemMap.at(lastElemIndex);
        ElemPtr lastElem = ElemAt(lastElemIndex);
        for (const auto& rtti : rttiVec) {
            rtti.MoveAssign(elem, rtti.Get(lastElem));
        }
        entityMap.erase(inEntity);
        entityMap.at(entityToLastElem) = elemIndex;
        elemMap.erase(lastElemIndex);
        elemMap.at(elemIndex) = entityToLastElem;
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

    size_t Archetype::Size() const
    {
        return size;
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
        const size_t newCapacity = static_cast<size_t>(static_cast<float>(Capacity()) * inRatio);
        std::vector<uint8_t> newMemory(newCapacity * elemSize);

        for (auto i = 0; i < size; i++) {
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
        if (Size() == Capacity()) {
            Reserve();
        }
        size++;
        return ElemAt(size - 1);
    }

    EntityPool::EntityPool()
        : counter()
    {
    }

    size_t EntityPool::Size() const
    {
        return allocated.size();
    }

    bool EntityPool::Valid(Entity inEntity) const
    {
        return allocated.contains(inEntity);
    }

    Entity EntityPool::Allocate()
    {
        if (!free.empty()) {
            const Entity result = *free.begin();
            free.erase(result);
            return result;
        } else {
            const Entity result = counter++;
            allocated.emplace(result);
            return result;
        }
    }

    void EntityPool::Free(Entity inEntity)
    {
        Assert(Valid(inEntity));
        allocated.erase(inEntity);
        free.emplace(inEntity);
    }

    void EntityPool::Clear()
    {
        counter = 0;
        free.clear();
        allocated.clear();
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

    RuntimeViewRule::RuntimeViewRule() = default;

    RuntimeViewRule& RuntimeViewRule::IncludeDyn(CompClass inClass)
    {
        includes.emplace(inClass);
        return *this;
    }

    RuntimeViewRule& RuntimeViewRule::ExcludeDyn(CompClass inClass)
    {
        excludes.emplace(inClass);
        return *this;
    }

    RuntimeView::RuntimeView(ECRegistry& inRegistry, const RuntimeViewRule& inArgs)
    {
        Evaluate(inRegistry, inArgs);
    }

    auto RuntimeView::Begin()
    {
        return resultEntities.begin();
    }

    auto RuntimeView::Begin() const
    {
        return resultEntities.begin();
    }

    auto RuntimeView::End()
    {
        return resultEntities.end();
    }

    auto RuntimeView::End() const
    {
        return resultEntities.end();
    }

    auto RuntimeView::begin()
    {
        return Begin();
    }

    auto RuntimeView::begin() const
    {
        return Begin();
    }

    auto RuntimeView::end()
    {
        return End();
    }

    auto RuntimeView::end() const
    {
        return End();
    }

    void RuntimeView::Evaluate(ECRegistry& inRegistry, const RuntimeViewRule& inArgs)
    {
        const std::vector includes(inArgs.includes.begin(), inArgs.includes.end());
        const std::vector excludes(inArgs.excludes.begin(), inArgs.excludes.end());

        slotMap.reserve(includes.size());
        for (auto i = 0; i < includes.size(); i++) {
            slotMap.emplace(includes[i], i);
        }

        for (auto& [_, archetype] : inRegistry.archetypes) {
            if (!archetype.ContainsAll(includes) || !archetype.NotContainsAny(excludes)) {
                continue;
            }

            resultEntities.reserve(result.size() + archetype.Size());
            result.reserve(result.size() + archetype.Size());
            for (const auto entity : archetype.All()) {
                std::vector<Mirror::Any> comps;
                comps.reserve(includes.size());
                for (const auto clazz : includes) {
                    comps.emplace_back(archetype.GetComp(entity, clazz));
                }

                resultEntities.emplace_back(entity);
                result.emplace_back(entity, std::move(comps));
            }
        }
    }

    Observer::Observer(ECRegistry& inRegistry)
        : registry(inRegistry)
    {
    }

    Observer::~Observer()
    {
        Reset();
    }

    void Observer::Each(const EntityTraverseFunc& inFunc) const
    {
        for (const auto& entity : entities) {
            inFunc(entity);
        }
    }

    void Observer::Clear()
    {
        entities.clear();
    }

    void Observer::Reset()
    {
        for (auto& [handle, deleter] : receiverHandles) {
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

    void Observer::OnEvent(Common::Event<ECRegistry&, Entity>& inEvent)
    {
        const auto handle = inEvent.BindMember<&Observer::RecordEntity>(*this);
        receiverHandles.emplace_back(
            handle,
            [&]() -> void {
                inEvent.Unbind(handle);
            });
    }

    ECRegistry::ECRegistry() = default;

    ECRegistry::~ECRegistry()
    {
        ResetRuntime();
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
        return entities.Allocate();
    }

    void ECRegistry::Destroy(Entity inEntity)
    {
        entities.Free(inEntity);
    }

    bool ECRegistry::Valid(Entity inEntity) const
    {
        return entities.Valid(inEntity);
    }

    size_t ECRegistry::Size() const
    {
        return entities.Size();
    }

    void ECRegistry::Clear()
    {
        entities.Clear();
        globalComps.clear();
        archetypes.clear();
        ResetRuntime();
    }

    void ECRegistry::ResetRuntime()
    {
        compEvents.clear();
        globalCompEvents.clear();
    }

    void ECRegistry::Each(const EntityTraverseFunc& inFunc) const
    {
        return entities.Each(inFunc);
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

    RuntimeView ECRegistry::RuntimeView(const RuntimeViewRule& inRule)
    {
        return Runtime::RuntimeView { *this, inRule };
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

    Mirror::Any ECRegistry::EmplaceDyn(CompClass inClass, Entity inEntity, const Mirror::ArgumentList& inArgs)
    {
        Assert(Valid(inEntity));
        const Internal::ArchetypeId archetypeId = entities.GetArchetype(inEntity);
        Internal::Archetype& archetype = archetypes.at(archetypeId);

        const Internal::ArchetypeId newArchetypeId = archetypeId + inClass->GetTypeInfo()->id;
        entities.SetArchetype(inEntity, newArchetypeId);

        Internal::Archetype* newArchetype;
        if (archetypes.contains(newArchetypeId)) {
            newArchetype = &archetypes.at(newArchetypeId);
            newArchetype->EmplaceElem(inEntity, archetype.GetElem(inEntity), archetype.GetRttiVec());
            archetype.EraseElem(inEntity);
        } else {
            archetypes.emplace(newArchetypeId, Internal::Archetype(archetype.NewRttiVecByAdd(Internal::CompRtti(inClass))));
            newArchetype = &archetypes.at(newArchetypeId);
            newArchetype->EmplaceElem(inEntity);
        }

        Mirror::Any tempObj = inClass->Construct(inArgs);
        Mirror::Any compRef = newArchetype->EmplaceComp(inEntity, inClass, tempObj.Ref());
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
        Assert(!GHasDyn(inClass));
        globalComps.emplace(inClass, inClass->ConstructDyn(inArgs));
        GNotifyConstructedDyn(inClass);
        return GGetDyn(inClass);
    }

    void ECRegistry::GRemoveDyn(GCompClass inClass)
    {
        Assert(GHasDyn(inClass));
        GNotifyRemoveDyn(inClass);
        globalComps.erase(inClass);
    }

    void ECRegistry::GUpdateDyn(GCompClass inClass, const DynUpdateFunc& inFunc)
    {
        Assert(GHasDyn(inClass));
        inFunc(GGetDyn(inClass));
        GNotifyUpdatedDyn(inClass);
    }

    GScopedUpdaterDyn ECRegistry::GUpdateDyn(GCompClass inClass)
    {
        Assert(GHasDyn(inClass));
        return { *this, inClass, GGetDyn(inClass) };
    }

    bool ECRegistry::GHasDyn(GCompClass inClass) const
    {
        return globalComps.contains(inClass);
    }

    Mirror::Any ECRegistry::GFindDyn(GCompClass inClass)
    {
        return GHasDyn(inClass) ? GGetDyn(inClass) : Mirror::Any();
    }

    Mirror::Any ECRegistry::GFindDyn(GCompClass inClass) const
    {
        return GHasDyn(inClass) ? GGetDyn(inClass) : Mirror::Any();
    }

    Mirror::Any ECRegistry::GGetDyn(GCompClass inClass)
    {
        Assert(GHasDyn(inClass));
        return globalComps.at(inClass);
    }

    Mirror::Any ECRegistry::GGetDyn(GCompClass inClass) const
    {
        Assert(GHasDyn(inClass));
        return globalComps.at(inClass).ConstRef();
    }

    ECRegistry::GCompEvents& ECRegistry::GEventsDyn(GCompClass inClass)
    {
        return globalCompEvents[inClass];
    }
} // namespace Runtime
