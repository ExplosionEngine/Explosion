//
// Created by johnk on 2024/10/31.
//

#include <Runtime/ECS.h>

namespace Runtime::Internal {
    void CompOp::SetOffset(size_t inOffset)
    {
        offset = inOffset;
    }

    void* CompOp::Emplace(void* inElem, void* inOther) const
    {
        return emplace(inElem, offset, inOther);
    }

    void CompOp::Destruct(void* inElem) const
    {
        destruct(inElem, offset);
    }

    void* CompOp::Get(void* inElem) const
    {
        return get(inElem, offset);
    }

    CompId CompOp::GetCompId() const
    {
        return compId;
    }

    size_t CompOp::GetOffset() const
    {
        return offset;
    }

    size_t CompOp::GetSize() const
    {
        return size;
    }

    CompOp::CompOp()
        : compId(0)
        , offset(0)
        , size(0)
        , emplace(nullptr)
        , destruct(nullptr)
        , get(nullptr)
    {
    }

    Archetype::Archetype(const std::vector<CompOp>& inCompOps)
        : archetypeId(0)
        , size(0)
        , elemSize(0)
        , compOps(inCompOps)
    {
        for (auto& compOp : compOps) {
            archetypeId += compOp.GetCompId();
            compOp.SetOffset(elemSize);
            elemSize += compOp.GetSize();
        }
    }

    bool Archetype::Contains(CompId inCompId) const
    {
        for (const auto& compOp : compOps) {
            if (compOp.GetCompId() == inCompId) {
                return true;
            }
        }
        return false;
    }

    bool Archetype::ContainsAll(const std::vector<CompId>& inCompIds) const
    {
        for (const auto& compId : inCompIds) {
            if (!Contains(compId)) {
                return false;
            }
        }
        return true;
    }

    bool Archetype::NotContainsAny(const std::vector<CompId>& inCompIds) const
    {
        for (const auto& compId : inCompIds) {
            if (Contains(compId)) {
                return false;
            }
        }
        return true;
    }

    void* Archetype::Emplace(Entity inEntity)
    {
        if (Size() == Capacity()) {
            Reserve();
        }
        auto* result = AllocateNewElemBack();
        auto backElem = BackElemIndex();
        entityMap.emplace(inEntity, backElem);
        elemMap.emplace(backElem, inEntity);
        return result;
    }

    void* Archetype::Emplace(Entity inEntity, void* inSourceElem, const std::vector<CompOp>& inSourceCompOps)
    {
        void* newElem = Emplace(inEntity);
        for (const auto& sourceCompOp : inSourceCompOps) {
            const auto* newCompOp = FindCompOp(sourceCompOp.GetCompId());
            if (newCompOp == nullptr) {
                continue;
            }
            newCompOp->Emplace(newElem, sourceCompOp.Get(inSourceElem));
        }
        return newElem;
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

    ECRegistry::~ECRegistry() = default;

    ECRegistry::ECRegistry(const ECRegistry& inOther)
        : entities(inOther.entities)
        , archetypes(inOther.archetypes)
        , compEvents(inOther.compEvents)
    {
    }

    ECRegistry::ECRegistry(ECRegistry&& inOther) noexcept
        : entities(std::move(inOther.entities))
        , archetypes(std::move(inOther.archetypes))
        , compEvents(std::move(inOther.compEvents))
    {
    }

    ECRegistry& ECRegistry::operator=(const ECRegistry& inOther)
    {
        entities = inOther.entities;
        archetypes = inOther.archetypes;
        compEvents = inOther.compEvents;
        return *this;
    }

    ECRegistry& ECRegistry::operator=(ECRegistry&& inOther) noexcept
    {
        entities = std::move(inOther.entities);
        archetypes = std::move(inOther.archetypes);
        compEvents = std::move(inOther.compEvents);
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
        archetypes.clear();
        compEvents.clear();
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

    Observer ECRegistry::Observer()
    {
        return Runtime::Observer { *this };
    }
} // namespace Runtime
