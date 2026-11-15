//
// Created by johnk on 2024/10/31.
//

#include <taskflow/taskflow.hpp>

#include <cstddef>
#include <cstring>
#include <new>
#include <utility>

#include <Core/Thread.h>
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

    TagStorage::TagStorage() = default;

    TagStorage::TagStorage(std::vector<TagClass> inTags)
        : tags(std::move(inTags))
        , tagSet(tags.begin(), tags.end())
    {
        Assert(tags.size() == tagSet.size());
    }

    bool TagStorage::Contains(TagClass inClass) const
    {
        return tagSet.contains(inClass);
    }

    size_t TagStorage::Count() const
    {
        return tags.size();
    }

    const std::vector<TagClass>& TagStorage::All() const
    {
        return tags;
    }

    TagStorage TagStorage::With(TagClass inClass) const
    {
        Assert(!Contains(inClass));
        auto result = tags;
        result.emplace_back(inClass);
        return TagStorage(std::move(result));
    }

    TagStorage TagStorage::Without(TagClass inClass) const
    {
        Assert(Contains(inClass));
        auto result = tags;
        const auto iter = std::ranges::find(result, inClass);
        Assert(iter != result.end());
        result.erase(iter);
        return TagStorage(std::move(result));
    }

    CompRtti::CompRtti(CompClass inClass)
        : clazz(inClass)
        , copyConstructFrom(nullptr)
        , moveConstructFrom(nullptr)
        , destruct(nullptr)
        , triviallyRelocatable(false)
    {
    }

    Mirror::Any CompRtti::CopyConstruct(ElemPtr inElem, const Mirror::Any& inOther) const
    {
        return clazz->GetConstructor(Mirror::IdPresets::copyCtor).InplaceNewDyn(inElem, { inOther.ConstRef() });
    }

    Mirror::Any CompRtti::MoveConstruct(ElemPtr inElem, const Mirror::Any& inOther) const
    {
        if (clazz->HasConstructor(Mirror::IdPresets::moveCtor)) {
            return clazz->GetConstructor(Mirror::IdPresets::moveCtor).InplaceNewDyn(inElem, { inOther });
        }
        return clazz->GetConstructor(Mirror::IdPresets::copyCtor).InplaceNewDyn(inElem, { inOther.ConstRef() });
    }

    void CompRtti::CopyConstructFrom(ElemPtr inDstElem, ElemPtr inSrcElem) const
    {
        if (copyConstructFrom != nullptr) {
            copyConstructFrom(inDstElem, inSrcElem);
        } else {
            CopyConstruct(inDstElem, Get(inSrcElem));
        }
    }

    void CompRtti::MoveConstructFrom(ElemPtr inDstElem, ElemPtr inSrcElem) const
    {
        if (moveConstructFrom != nullptr) {
            moveConstructFrom(inDstElem, inSrcElem);
        } else {
            MoveConstruct(inDstElem, Get(inSrcElem));
        }
    }

    void CompRtti::Destruct(ElemPtr inElem) const
    {
        if (destruct != nullptr) {
            destruct(inElem);
        } else {
            clazz->DestructDyn(clazz->InplaceGetObject(inElem));
        }
    }

    Mirror::Any CompRtti::Get(ElemPtr inElem) const
    {
        return clazz->InplaceGetObject(inElem);
    }

    size_t CompRtti::MemorySize() const
    {
        return clazz->SizeOf();
    }

    size_t CompRtti::MemoryAlignment() const
    {
        return clazz->AlignOf();
    }

    ArchetypeLayout::ArchetypeLayout()
        : id(0)
    {
    }

    ArchetypeLayout::ArchetypeLayout(std::vector<CompRtti> inCompRttis, TagStorage inTags)
        : id(0)
        , compRttis(std::move(inCompRttis))
        , tags(std::move(inTags))
    {
        for (const auto& rtti : compRttis) {
            id += rtti.Class()->GetTypeInfo()->id;
        }
        for (const auto* tag : tags.All()) {
            id += tag->GetTypeInfo()->id;
        }
    }

    bool ArchetypeLayout::ContainsComp(CompClass inClass) const
    {
        return std::ranges::any_of(compRttis, [inClass](const CompRtti& rtti) -> bool { return rtti.Class() == inClass; });
    }

    bool ArchetypeLayout::ContainsTag(TagClass inClass) const
    {
        return tags.Contains(inClass);
    }

    bool ArchetypeLayout::Contains(CompClass inClass) const
    {
        return ContainsComp(inClass) || ContainsTag(inClass);
    }

    ArchetypeId ArchetypeLayout::Id() const
    {
        return id;
    }

    const std::vector<CompRtti>& ArchetypeLayout::CompRttis() const
    {
        return compRttis;
    }

    const TagStorage& ArchetypeLayout::Tags() const
    {
        return tags;
    }

    ArchetypeLayout ArchetypeLayout::WithComp(const CompRtti& inRtti) const
    {
        Assert(!Contains(inRtti.Class()));
        auto result = compRttis;
        result.emplace_back(inRtti);
        return ArchetypeLayout(std::move(result), tags);
    }

    ArchetypeLayout ArchetypeLayout::WithTag(TagClass inClass) const
    {
        Assert(!Contains(inClass));
        return ArchetypeLayout(compRttis, tags.With(inClass));
    }

    ArchetypeLayout ArchetypeLayout::Without(CompClass inClass) const
    {
        Assert(Contains(inClass));
        if (tags.Contains(inClass)) {
            return ArchetypeLayout(compRttis, tags.Without(inClass));
        }

        auto result = compRttis;
        const auto iter = std::ranges::find_if(result, [inClass](const CompRtti& rtti) -> bool { return rtti.Class() == inClass; });
        Assert(iter != result.end());
        result.erase(iter);
        return ArchetypeLayout(std::move(result), tags);
    }

    Archetype::Archetype(ArchetypeLayout inLayout)
        : id(inLayout.Id())
        , count(0)
        , capacity(0)
        , rttiVec(inLayout.CompRttis())
        , tags(inLayout.Tags())
        , compMemory(rttiVec.size(), nullptr)
        , compStrides(rttiVec.size())
    {
        rttiMap.reserve(rttiVec.size());
        for (auto i = 0; i < rttiVec.size(); i++) {
            auto& rtti = rttiVec[i];
            const auto clazz = rtti.Class();
            rttiMap.emplace(clazz, i);
            compStrides[i] = rtti.MemorySize();

        }
    }

    Archetype::~Archetype()
    {
        DestroyElements();
        ReleaseMemory();
    }

    Archetype::Archetype(const Archetype& inOther)
        : id(inOther.id)
        , count(inOther.count)
        , capacity(inOther.capacity)
        , rttiVec(inOther.rttiVec)
        , tags(inOther.tags)
        , rttiMap(inOther.rttiMap)
        , compMemory(rttiVec.size(), nullptr)
        , compStrides(inOther.compStrides)
        , elemMap(inOther.elemMap)
    {
        for (size_t compIndex = 0; compIndex < rttiVec.size(); compIndex++) {
            const auto& rtti = rttiVec[compIndex];
            if (capacity > 0) {
                compMemory[compIndex] = ::operator new(capacity * rtti.MemorySize(), std::align_val_t(rtti.MemoryAlignment()));
            }
            if (rtti.TriviallyRelocatable()) {
                if (count > 0) {
                    std::memcpy(compMemory[compIndex], inOther.compMemory[compIndex], count * compStrides[compIndex]);
                }
            } else {
                for (size_t elemIndex = 0; elemIndex < count; elemIndex++) {
                    rtti.CopyConstructFrom(GetCompAt(elemIndex, compIndex), const_cast<void*>(inOther.GetCompAt(elemIndex, compIndex)));
                }
            }
        }
    }

    Archetype::Archetype(Archetype&& inOther) noexcept
        : id(inOther.id)
        , count(std::exchange(inOther.count, 0))
        , capacity(std::exchange(inOther.capacity, 0))
        , rttiVec(std::move(inOther.rttiVec))
        , tags(std::move(inOther.tags))
        , rttiMap(std::move(inOther.rttiMap))
        , compMemory(std::move(inOther.compMemory))
        , compStrides(std::move(inOther.compStrides))
        , elemMap(std::move(inOther.elemMap))
    {
    }

    Archetype& Archetype::operator=(const Archetype& inOther)
    {
        if (this != &inOther) {
            *this = Archetype(inOther);
        }
        return *this;
    }

    Archetype& Archetype::operator=(Archetype&& inOther) noexcept
    {
        if (this == &inOther) {
            return *this;
        }

        DestroyElements();
        ReleaseMemory();
        id = inOther.id;
        count = std::exchange(inOther.count, 0);
        capacity = std::exchange(inOther.capacity, 0);
        rttiVec = std::move(inOther.rttiVec);
        tags = std::move(inOther.tags);
        rttiMap = std::move(inOther.rttiMap);
        compMemory = std::move(inOther.compMemory);
        compStrides = std::move(inOther.compStrides);
        elemMap = std::move(inOther.elemMap);
        addTransitions.clear();
        removeTransitions.clear();
        return *this;
    }

    bool Archetype::ContainsComp(CompClass inClass) const
    {
        return rttiMap.contains(inClass);
    }

    bool Archetype::ContainsTag(TagClass inClass) const
    {
        return tags.Contains(inClass);
    }

    bool Archetype::Contains(CompClass inClazz) const
    {
        return ContainsComp(inClazz) || ContainsTag(inClazz);
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

    size_t Archetype::EmplaceElem(Entity inEntity)
    {
        AllocateNewElemBack();
        elemMap.emplace_back(inEntity);
        return count - 1;
    }

    size_t Archetype::EmplaceElem(Entity inEntity, Archetype& inSrcArchetype, size_t inSrcElemIndex, const std::vector<CompMapping>& inCompMappings)
    {
        const auto newElemIndex = EmplaceElem(inEntity);
        for (const auto& mapping : inCompMappings) {
            const auto& dstRtti = rttiVec[mapping.dstCompIndex];
            if (dstRtti.TriviallyRelocatable()) {
                std::memcpy(GetCompAt(newElemIndex, mapping.dstCompIndex), inSrcArchetype.GetCompAt(inSrcElemIndex, mapping.srcCompIndex), compStrides[mapping.dstCompIndex]);
            } else {
                dstRtti.MoveConstructFrom(GetCompAt(newElemIndex, mapping.dstCompIndex), inSrcArchetype.GetCompAt(inSrcElemIndex, mapping.srcCompIndex));
            }
        }
        return newElemIndex;
    }

    Mirror::Any Archetype::EmplaceComp(size_t inElemIndex, CompClass inCompClass, const Mirror::Any& inCompRef) // NOLINT
    {
        Assert(inElemIndex < count);
        const auto compIndex = GetCompIndex(inCompClass);
        return rttiVec[compIndex].MoveConstruct(GetCompAt(inElemIndex, compIndex), inCompRef);
    }

    Entity Archetype::EraseElem(size_t inElemIndex)
    {
        Assert(inElemIndex < count);
        const auto lastElemIndex = count - 1;
        Entity movedEntity = entityNull;

        if (inElemIndex == lastElemIndex) {
            for (size_t compIndex = 0; compIndex < rttiVec.size(); compIndex++) {
                if (!rttiVec[compIndex].TriviallyRelocatable()) {
                    rttiVec[compIndex].Destruct(GetCompAt(inElemIndex, compIndex));
                }
            }
        } else {
            for (size_t compIndex = 0; compIndex < rttiVec.size(); compIndex++) {
                const auto& rtti = rttiVec[compIndex];
                ElemPtr elem = GetCompAt(inElemIndex, compIndex);
                ElemPtr lastElem = GetCompAt(lastElemIndex, compIndex);
                if (rtti.TriviallyRelocatable()) {
                    std::memcpy(elem, lastElem, compStrides[compIndex]);
                } else {
                    rtti.Destruct(elem);
                    rtti.MoveConstructFrom(elem, lastElem);
                    rtti.Destruct(lastElem);
                }
            }

            const auto entityToLastElem = elemMap.at(lastElemIndex);
            elemMap[inElemIndex] = entityToLastElem;
            movedEntity = entityToLastElem;
        }

        elemMap.pop_back();
        count--;
        return movedEntity;
    }

    Mirror::Any Archetype::GetComp(size_t inElemIndex, CompClass inCompClass)
    {
        const auto compIndex = GetCompIndex(inCompClass);
        return rttiVec[compIndex].Get(GetCompAt(inElemIndex, compIndex));
    }

    Mirror::Any Archetype::GetComp(size_t inElemIndex, CompClass inCompClass) const
    {
        const auto compIndex = GetCompIndex(inCompClass);
        return rttiVec[compIndex].Get(const_cast<ElemPtr>(GetCompAt(inElemIndex, compIndex))).ConstRef();
    }

    size_t Archetype::GetCompIndex(CompClass inCompClass) const
    {
        const auto iter = rttiMap.find(inCompClass);
        Assert(iter != rttiMap.end());
        return iter->second;
    }

    size_t Archetype::Count() const
    {
        return count;
    }

    const std::vector<CompRtti>& Archetype::GetCompRttis() const
    {
        return rttiVec;
    }

    const TagStorage& Archetype::GetTags() const
    {
        return tags;
    }

    ArchetypeLayout Archetype::GetLayout() const
    {
        return ArchetypeLayout(rttiVec, tags);
    }

    ArchetypeId Archetype::Id() const
    {
        return id;
    }

    const Archetype::Transition* Archetype::FindAddTransition(CompClass inClass) const
    {
        const auto iter = std::ranges::find_if(addTransitions, [&](const Transition& transition) -> bool { return transition.compClass == inClass; });
        return iter != addTransitions.end() ? &*iter : nullptr;
    }

    const Archetype::Transition* Archetype::FindRemoveTransition(CompClass inClass) const
    {
        const auto iter = std::ranges::find_if(removeTransitions, [&](const Transition& transition) -> bool { return transition.compClass == inClass; });
        return iter != removeTransitions.end() ? &*iter : nullptr;
    }

    const Archetype::Transition& Archetype::CacheAddTransition(CompClass inClass, Archetype& inArchetype)
    {
        Assert(FindAddTransition(inClass) == nullptr);
        return CacheTransition(addTransitions, inClass, inArchetype);
    }

    const Archetype::Transition& Archetype::CacheRemoveTransition(CompClass inClass, Archetype& inArchetype)
    {
        Assert(FindRemoveTransition(inClass) == nullptr);
        return CacheTransition(removeTransitions, inClass, inArchetype);
    }

    const Archetype::Transition& Archetype::CacheTransition(std::vector<Transition>& inTransitions, CompClass inClass, Archetype& inArchetype)
    {
        auto& transition = inTransitions.emplace_back(Transition { inClass, &inArchetype, {} });
        transition.compMappings.reserve(rttiVec.size());
        for (size_t srcCompIndex = 0; srcCompIndex < rttiVec.size(); srcCompIndex++) {
            const auto dstIter = inArchetype.rttiMap.find(rttiVec[srcCompIndex].Class());
            if (dstIter != inArchetype.rttiMap.end()) {
                transition.compMappings.emplace_back(srcCompIndex, dstIter->second);
            }
        }
        return transition;
    }

    size_t Archetype::Capacity() const
    {
        return capacity;
    }

    void Archetype::Reserve(float inRatio)
    {
        Assert(inRatio > 1.0f);
        const size_t newCapacity = static_cast<size_t>(std::ceil(static_cast<float>(std::max(Capacity(), static_cast<size_t>(1))) * inRatio));
        std::vector<ElemPtr> newCompMemory(rttiVec.size(), nullptr);

        for (size_t compIndex = 0; compIndex < rttiVec.size(); compIndex++) {
            const auto& rtti = rttiVec[compIndex];
            newCompMemory[compIndex] = ::operator new(newCapacity * rtti.MemorySize(), std::align_val_t(rtti.MemoryAlignment()));
            if (rtti.TriviallyRelocatable()) {
                if (count > 0) {
                    std::memcpy(newCompMemory[compIndex], compMemory[compIndex], count * compStrides[compIndex]);
                }
            } else {
                for (size_t elemIndex = 0; elemIndex < count; elemIndex++) {
                    ElemPtr dstElem = CompAt(newCompMemory[compIndex], compIndex, elemIndex);
                    ElemPtr srcElem = GetCompAt(elemIndex, compIndex);
                    rtti.MoveConstructFrom(dstElem, srcElem);
                    rtti.Destruct(srcElem);
                }
            }
            if (compMemory[compIndex] != nullptr) {
                ::operator delete(compMemory[compIndex], std::align_val_t(rtti.MemoryAlignment()));
            }
        }
        compMemory = std::move(newCompMemory);
        capacity = newCapacity;
    }

    void Archetype::DestroyElements()
    {
        for (size_t compIndex = 0; compIndex < rttiVec.size(); compIndex++) {
            const auto& rtti = rttiVec[compIndex];
            if (rtti.TriviallyRelocatable()) {
                continue;
            }
            for (size_t elemIndex = 0; elemIndex < count; elemIndex++) {
                rtti.Destruct(GetCompAt(elemIndex, compIndex));
            }
        }
        count = 0;
    }

    void Archetype::ReleaseMemory()
    {
        for (size_t compIndex = 0; compIndex < compMemory.size(); compIndex++) {
            if (compMemory[compIndex] != nullptr) {
                ::operator delete(compMemory[compIndex], std::align_val_t(rttiVec[compIndex].MemoryAlignment()));
                compMemory[compIndex] = nullptr;
            }
        }
        capacity = 0;
    }

    void Archetype::AllocateNewElemBack()
    {
        if (Count() == Capacity()) {
            Reserve();
        }
        count++;
    }

    EntityPool::EntityPool()
        : locations(1)
        , states(1)
    {
    }

    size_t EntityPool::Count() const
    {
        return allocated.size();
    }

    bool EntityPool::Valid(Entity inEntity) const
    {
        return inEntity < locations.size() && locations[inEntity].archetype != nullptr;
    }

    Entity EntityPool::Allocate()
    {
        Entity result;
        if (!free.empty()) {
            result = free.back();
            free.pop_back();
        } else {
            Assert(locations.size() < invalidIndex);
            result = static_cast<Entity>(locations.size());
            locations.emplace_back();
            states.emplace_back();
        }
        locations[result] = {};
        states[result].archetype = 0;
        Assert(allocated.size() < invalidIndex);
        states[result].allocatedIndex = static_cast<Entity>(allocated.size());
        allocated.emplace_back(result);
        return result;
    }

    void EntityPool::Allocate(Entity inEntity)
    {
        if (inEntity < locations.size()) {
            Assert(states[inEntity].allocatedIndex == invalidIndex);
            const auto iter = std::ranges::find(free, inEntity);
            Assert(iter != free.end());
            *iter = free.back();
            free.pop_back();
        } else {
            for (Entity i = static_cast<Entity>(locations.size()); i < inEntity; i++) {
                free.emplace_back(i);
            }
            const size_t newSize = static_cast<size_t>(inEntity) + 1;
            locations.resize(newSize);
            states.resize(newSize);
        }
        locations[inEntity] = {};
        states[inEntity].archetype = 0;
        Assert(allocated.size() < invalidIndex);
        states[inEntity].allocatedIndex = static_cast<Entity>(allocated.size());
        allocated.emplace_back(inEntity);
    }

    void EntityPool::Free(Entity inEntity)
    {
        Assert(Valid(inEntity));
        const auto lastEntity = allocated.back();
        allocated[states[inEntity].allocatedIndex] = lastEntity;
        states[lastEntity].allocatedIndex = states[inEntity].allocatedIndex;
        allocated.pop_back();
        locations[inEntity] = {};
        states[inEntity] = {};
        free.emplace_back(inEntity);
    }

    void EntityPool::Clear()
    {
        locations.assign(1, {});
        states.assign(1, {});
        free.clear();
        allocated.clear();
    }

    void EntityPool::Each(const EntityTraverseFunc& inFunc) const
    {
        for (const auto& entity : allocated) {
            inFunc(entity);
        }
    }

    void EntityPool::SetLocation(Entity inEntity, Archetype& inArchetype, size_t inElemIndex)
    {
        Assert(inEntity < states.size() && states[inEntity].allocatedIndex != invalidIndex && inElemIndex < invalidIndex);
        states[inEntity].archetype = inArchetype.Id();
        locations[inEntity] = { &inArchetype, static_cast<Entity>(inElemIndex) };
    }

    void EntityPool::SetElemIndex(Entity inEntity, size_t inElemIndex)
    {
        Assert(Valid(inEntity) && inElemIndex < invalidIndex);
        locations[inEntity].elemIndex = static_cast<Entity>(inElemIndex);
    }

    void EntityPool::SetArchetypePtr(Entity inEntity, Archetype& inArchetype)
    {
        Assert(Valid(inEntity) && states[inEntity].archetype == inArchetype.Id());
        locations[inEntity].archetype = &inArchetype;
    }

    ArchetypeId EntityPool::GetArchetype(Entity inEntity) const
    {
        Assert(Valid(inEntity));
        return states[inEntity].archetype;
    }

    Archetype* EntityPool::GetArchetypePtr(Entity inEntity)
    {
        Assert(Valid(inEntity));
        return locations[inEntity].archetype;
    }

    const Archetype* EntityPool::GetArchetypePtr(Entity inEntity) const
    {
        Assert(Valid(inEntity));
        return locations[inEntity].archetype;
    }

    size_t EntityPool::GetElemIndex(Entity inEntity) const
    {
        Assert(Valid(inEntity));
        return locations[inEntity].elemIndex;
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

    RuntimeFilter& RuntimeFilter::IncludeTagDyn(TagClass inClass)
    {
        Assert(inClass->SizeOf() == 1 && inClass->AlignOf() == 1);
        Assert(!tagIncludes.contains(inClass));
        tagIncludes.emplace(inClass);
        return *this;
    }

    RuntimeFilter& RuntimeFilter::ExcludeTagDyn(TagClass inClass)
    {
        Assert(inClass->SizeOf() == 1 && inClass->AlignOf() == 1);
        Assert(!tagExcludes.contains(inClass));
        tagExcludes.emplace(inClass);
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

    const Observer& EventsObserverDyn::Constructed() const
    {
        return constructedObserver;
    }

    const Observer& EventsObserverDyn::Updated() const
    {
        return updatedObserver;
    }

    const Observer& EventsObserverDyn::Removed() const
    {
        return removedObserver;
    }

    ECRegistry::ECRegistry()
    {
        archetypes.emplace(0, Internal::Archetype());
    }

    ECRegistry::~ECRegistry()
    {
        CheckEventsUnbound();
    }

    ECRegistry::ECRegistry(const ECRegistry& inOther)
        : entities(inOther.entities)
        , globalComps(inOther.globalComps)
        , archetypes(inOther.archetypes)
        , dataCompClasses(inOther.dataCompClasses)
        , tagClasses(inOther.tagClasses)
    {
        RebindEntityArchetypes();
    }

    ECRegistry::ECRegistry(ECRegistry&& inOther) noexcept
        : entities(std::move(inOther.entities))
        , globalComps(std::move(inOther.globalComps))
        , archetypes(std::move(inOther.archetypes))
        , dataCompClasses(std::move(inOther.dataCompClasses))
        , tagClasses(std::move(inOther.tagClasses))
    {
        RebindEntityArchetypes();
    }

    ECRegistry& ECRegistry::operator=(const ECRegistry& inOther)
    {
        if (this == &inOther) {
            return *this;
        }
        entities = inOther.entities;
        globalComps = inOther.globalComps;
        archetypes = inOther.archetypes;
        dataCompClasses = inOther.dataCompClasses;
        tagClasses = inOther.tagClasses;
        RebindEntityArchetypes();
        return *this;
    }

    ECRegistry& ECRegistry::operator=(ECRegistry&& inOther) noexcept
    {
        if (this == &inOther) {
            return *this;
        }
        entities = std::move(inOther.entities);
        globalComps = std::move(inOther.globalComps);
        archetypes = std::move(inOther.archetypes);
        dataCompClasses = std::move(inOther.dataCompClasses);
        tagClasses = std::move(inOther.tagClasses);
        RebindEntityArchetypes();
        return *this;
    }

    Entity ECRegistry::Create()
    {
        const Entity result = entities.Allocate();
        Internal::Archetype& archetype = archetypes.at(0);
        const auto elemIndex = archetype.EmplaceElem(result);
        entities.SetLocation(result, archetype, elemIndex);
        return result;
    }

    void ECRegistry::Create(Entity inEntity)
    {
        entities.Allocate(inEntity);
        Internal::Archetype& archetype = archetypes.at(0);
        const auto elemIndex = archetype.EmplaceElem(inEntity);
        entities.SetLocation(inEntity, archetype, elemIndex);
    }

    void ECRegistry::Destroy(Entity inEntity)
    {
        const auto location = entities.GetLocation(inEntity);
        Internal::Archetype& archetype = *location.archetype;
        if (!compEvents.empty()) {
            for (const auto& compRtti : archetype.GetCompRttis()) {
                NotifyRemoveDyn(compRtti.Class(), inEntity);
            }
            for (const auto* tag : archetype.GetTags().All()) {
                NotifyRemoveDyn(tag, inEntity);
            }
        }
        EraseArchetypeElem(archetype, location.elemIndex);
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
        dataCompClasses.clear();
        tagClasses.clear();
        archetypes.emplace(0, Internal::Archetype());
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
        const Internal::Archetype& archetype = *entities.GetArchetypePtr(inEntity);
        for (const auto& compRtti : archetype.GetCompRttis()) {
            inFunc(compRtti.Class());
        }
    }

    size_t ECRegistry::CompCount(Entity inEntity) const
    {
        const Internal::Archetype& archetype = *entities.GetArchetypePtr(inEntity);
        return archetype.GetCompRttis().size();
    }

    void ECRegistry::TagEach(Entity inEntity, const TagTraverseFunc& inFunc) const
    {
        const Internal::Archetype& archetype = *entities.GetArchetypePtr(inEntity);
        for (const auto* tag : archetype.GetTags().All()) {
            inFunc(tag);
        }
    }

    size_t ECRegistry::TagCount(Entity inEntity) const
    {
        return entities.GetArchetypePtr(inEntity)->GetTags().Count();
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

    Runtime::EventsObserverDyn ECRegistry::EventsObserverDyn(CompClass inClass)
    {
        return Runtime::EventsObserverDyn { *this, inClass };
    }

    void ECRegistry::NotifyUpdatedDyn(CompClass inClass, Entity inEntity)
    {
        if (compEvents.empty()) {
            return;
        }
        const auto iter = compEvents.find(inClass);
        if (iter == compEvents.end()) {
            return;
        }
        iter->second.onUpdated.Broadcast(*this, inEntity);
    }

    void ECRegistry::NotifyConstructedDyn(CompClass inClass, Entity inEntity)
    {
        if (compEvents.empty()) {
            return;
        }
        const auto iter = compEvents.find(inClass);
        if (iter == compEvents.end()) {
            return;
        }
        iter->second.onConstructed.Broadcast(*this, inEntity);
    }

    void ECRegistry::NotifyRemoveDyn(CompClass inClass, Entity inEntity)
    {
        if (compEvents.empty()) {
            return;
        }
        const auto iter = compEvents.find(inClass);
        if (iter == compEvents.end()) {
            return;
        }
        iter->second.onRemove.Broadcast(*this, inEntity);
    }

    Runtime::Observer ECRegistry::Observer()
    {
        return Runtime::Observer { *this };
    }

    void ECRegistry::Save(ECArchive& outArchive) const
    {
        outArchive = {};
        outArchive.entities.reserve(Count());
        Each([&](Entity entity) -> void {
            if (HasTag<TransientTag>(entity)) {
                return;
            }
            outArchive.entities.emplace(entity, EntityArchive {});
            auto& entityArchive = outArchive.entities.at(entity);
            auto& comps = entityArchive.comps;
            comps.reserve(CompCount(entity));

            CompEach(entity, [&](CompClass clazz) -> void {
                if (clazz->IsTransient()) {
                    return;
                }
                comps.emplace(clazz, std::vector<uint8_t> {});
                Common::MemorySerializeStream stream(comps.at(clazz));
                GetDyn(clazz, entity).Serialize(stream);
            });

            entityArchive.tags.reserve(TagCount(entity));
            TagEach(entity, [&](TagClass tag) -> void {
                if (!tag->IsTransient()) {
                    entityArchive.tags.emplace_back(tag);
                }
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
            for (const auto* tag : entityArchive.tags) {
                if (!tag->IsTransient()) {
                    AddTagDyn(tag, entity);
                }
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

    void ECRegistry::EraseArchetypeElem(Internal::Archetype& inArchetype, size_t inElemIndex)
    {
        const Entity movedEntity = inArchetype.EraseElem(inElemIndex);
        if (movedEntity != entityNull) {
            entities.SetElemIndex(movedEntity, inElemIndex);
        }
    }

    void ECRegistry::RebindEntityArchetypes()
    {
        for (auto iter = entities.Begin(); iter != entities.End(); ++iter) {
            entities.SetArchetypePtr(*iter, archetypes.at(entities.GetArchetype(*iter)));
        }
    }

    void ECRegistry::RegisterDataCompClass(CompClass inClass)
    {
        Assert(!tagClasses.contains(inClass));
        dataCompClasses.emplace(inClass);
    }

    void ECRegistry::RegisterTagClass(TagClass inClass)
    {
        Assert(!dataCompClasses.contains(inClass));
        tagClasses.emplace(inClass);
    }

    Mirror::Any ECRegistry::EmplaceDyn(CompClass inClass, Entity inEntity, const Mirror::ArgumentList& inArgs)
    {
        const auto location = MoveEntityForAdd(Internal::CompRtti(inClass), inEntity);
        Mirror::Any tempObj = inClass->ConstructDyn(inArgs);
        Mirror::Any compRef = location.archetype->EmplaceComp(location.elemIndex, inClass, tempObj.Ref());
        NotifyConstructedDyn(inClass, inEntity);
        return compRef;
    }

    void ECRegistry::RemoveDyn(CompClass inClass, Entity inEntity)
    {
        Assert(Valid(inEntity) && HasDyn(inClass, inEntity));
        MoveEntityForRemove(inClass, inEntity);
    }

    Internal::EntityPool::Location ECRegistry::MoveEntityForAdd(const Internal::CompRtti& inRtti, Entity inEntity)
    {
        const CompClass inClass = inRtti.Class();
        RegisterDataCompClass(inClass);
        const auto location = entities.GetLocation(inEntity);
        if (const auto* transition = location.archetype->FindAddTransition(inClass)) {
            return MoveEntityThroughTransition(inEntity, location, *transition);
        }
        return MoveEntityForAdd(inClass, inEntity, location, location.archetype->GetLayout().WithComp(inRtti));
    }

    Internal::EntityPool::Location ECRegistry::MoveEntityForAddTag(TagClass inClass, Entity inEntity)
    {
        RegisterTagClass(inClass);
        const auto location = entities.GetLocation(inEntity);
        if (const auto* transition = location.archetype->FindAddTransition(inClass)) {
            return MoveEntityThroughTransition(inEntity, location, *transition);
        }
        return MoveEntityForAdd(inClass, inEntity, location, location.archetype->GetLayout().WithTag(inClass));
    }

    Internal::EntityPool::Location ECRegistry::MoveEntityForAdd(CompClass inClass, Entity inEntity, const Internal::EntityPool::Location& inLocation, Internal::ArchetypeLayout inLayout)
    {
        Internal::Archetype& archetype = *inLocation.archetype;
        Assert(archetype.FindAddTransition(inClass) == nullptr);
        const Internal::ArchetypeId newArchetypeId = inLayout.Id();
        if (!archetypes.contains(newArchetypeId)) {
            archetypes.emplace(newArchetypeId, Internal::Archetype(std::move(inLayout)));
        }
        Internal::Archetype& newArchetype = archetypes.at(newArchetypeId);
        const auto& transition = archetype.CacheAddTransition(inClass, newArchetype);
        if (newArchetype.FindRemoveTransition(inClass) == nullptr) {
            newArchetype.CacheRemoveTransition(inClass, archetype);
        }

        return MoveEntityThroughTransition(inEntity, inLocation, transition);
    }

    Internal::EntityPool::Location ECRegistry::MoveEntityThroughTransition(Entity inEntity, const Internal::EntityPool::Location& inLocation, const Internal::Archetype::Transition& inTransition)
    {
        Internal::Archetype& archetype = *inLocation.archetype;
        Internal::Archetype& newArchetype = *inTransition.archetype;
        const auto newElemIndex = newArchetype.EmplaceElem(inEntity, archetype, inLocation.elemIndex, inTransition.compMappings);
        entities.SetLocation(inEntity, newArchetype, newElemIndex);
        EraseArchetypeElem(archetype, inLocation.elemIndex);
        return { &newArchetype, newElemIndex };
    }

    void ECRegistry::MoveEntityForRemove(CompClass inClass, Entity inEntity)
    {
        const auto location = entities.GetLocation(inEntity);
        Internal::Archetype& archetype = *location.archetype;
        const Internal::Archetype::Transition* transition = archetype.FindRemoveTransition(inClass);

        if (transition == nullptr) {
            Assert(archetype.Contains(inClass));
            Internal::ArchetypeLayout newLayout = archetype.GetLayout().Without(inClass);
            const Internal::ArchetypeId newArchetypeId = newLayout.Id();
            if (!archetypes.contains(newArchetypeId)) {
                archetypes.emplace(newArchetypeId, Internal::Archetype(std::move(newLayout)));
            }
            Internal::Archetype& newArchetype = archetypes.at(newArchetypeId);
            transition = &archetype.CacheRemoveTransition(inClass, newArchetype);
            if (newArchetype.FindAddTransition(inClass) == nullptr) {
                newArchetype.CacheAddTransition(inClass, archetype);
            }
        }

        if (!compEvents.empty()) {
            NotifyRemoveDyn(inClass, inEntity);
        }
        MoveEntityThroughTransition(inEntity, location, *transition);
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
        return entities.GetArchetypePtr(inEntity)->ContainsComp(inClass);
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
        Mirror::Any compRef = entities.GetArchetypePtr(inEntity)->GetComp(entities.GetElemIndex(inEntity), inClass);
        return compRef;
    }

    Mirror::Any ECRegistry::GetDyn(CompClass inClass, Entity inEntity) const
    {
        Assert(Valid(inEntity) && HasDyn(inClass, inEntity));
        Mirror::Any compRef = entities.GetArchetypePtr(inEntity)->GetComp(entities.GetElemIndex(inEntity), inClass);
        return compRef.ConstRef();
    }

    ECRegistry::CompEvents& ECRegistry::EventsDyn(CompClass inClass)
    {
        return compEvents[inClass];
    }

    void ECRegistry::AddTagDyn(TagClass inClass, Entity inEntity)
    {
        Assert(inClass->SizeOf() == 1 && inClass->AlignOf() == 1);
        Assert(Valid(inEntity) && !HasTagDyn(inClass, inEntity));
        MoveEntityForAddTag(inClass, inEntity);
        NotifyConstructedDyn(inClass, inEntity);
    }

    void ECRegistry::RemoveTagDyn(TagClass inClass, Entity inEntity)
    {
        Assert(Valid(inEntity) && HasTagDyn(inClass, inEntity));
        MoveEntityForRemove(inClass, inEntity);
    }

    bool ECRegistry::HasTagDyn(TagClass inClass, Entity inEntity) const
    {
        Assert(Valid(inEntity));
        return entities.GetArchetypePtr(inEntity)->GetTags().Contains(inClass);
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
        auto lastBarrier = taskFlow.emplace([]() -> void { Core::ScopedThreadTag threadTag(Core::ThreadTag::gameWorker); });

        for (auto& groupContext : systemGraph) {
            if (groupContext.strategy == SystemExecuteStrategy::sequential) {
                tf::Task lastTask = lastBarrier;
                for (auto& systemContext : groupContext.systems) {
                    auto task = taskFlow.emplace([&]() -> void {
                        Core::ScopedThreadTag threadTag(Core::ThreadTag::gameWorker);
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
                        Core::ScopedThreadTag threadTag(Core::ThreadTag::gameWorker);
                        inActionFunc(systemContext);
                    }));
                    tasks.back().succeed(lastBarrier);
                }

                auto barrier = taskFlow.emplace([]() -> void { Core::ScopedThreadTag threadTag(Core::ThreadTag::gameWorker); });
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
