//
// Created by johnk on 2024/10/14.
//

#include <utility>

#include <Runtime/Component/Transform.h>

namespace Runtime {
    WorldTransform::WorldTransform() = default;

    WorldTransform::WorldTransform(Common::FTransform inLocalToWorld)
        : localToWorld(std::move(inLocalToWorld))
    {
    }

    LocalTransform::LocalTransform() = default;

    LocalTransform::LocalTransform(Common::FTransform inLocalToParent)
        : localToParent(std::move(inLocalToParent))
    {
    }

    Hierarchy::Hierarchy()
        : parent(entityNull)
        , firstChild(entityNull)
        , prevBro(entityNull)
        , nextBro(entityNull)
    {
    }

    bool HierarchyOps::HasParent(ECRegistry& inRegistry, Entity inTarget)
    {
        const auto& hierarchy = inRegistry.Get<Hierarchy>(inTarget);
        return hierarchy.parent != entityNull;
    }

    bool HierarchyOps::HasBro(ECRegistry& inRegistry, Entity inTarget)
    {
        const auto& hierarchy = inRegistry.Get<Hierarchy>(inTarget);
        return hierarchy.prevBro != entityNull || hierarchy.nextBro != entityNull;
    }

    bool HierarchyOps::HasChildren(ECRegistry& inRegistry, Entity inTarget)
    {
        const auto& hierarchy = inRegistry.Get<Hierarchy>(inTarget);
        return hierarchy.firstChild != entityNull;
    }

    void HierarchyOps::AttachToParent(ECRegistry& inRegistry, Entity inChild, Entity inParent)
    {
        Assert(!HasParent(inRegistry, inChild) && !HasBro(inRegistry, inChild));
        auto& childHierarchy = inRegistry.Get<Hierarchy>(inChild);
        auto& parentHierarchy = inRegistry.Get<Hierarchy>(inParent);
        childHierarchy.parent = inParent;
        childHierarchy.nextBro = parentHierarchy.firstChild;
        if (parentHierarchy.firstChild != entityNull) {
            auto& oldFirstChildHierarchy = inRegistry.Get<Hierarchy>(parentHierarchy.firstChild);
            oldFirstChildHierarchy.prevBro = inChild;
        }
        parentHierarchy.firstChild = inChild;
    }

    void HierarchyOps::DetachFromParent(ECRegistry& inRegistry, Entity inChild)
    {
        Assert(HasParent(inRegistry, inChild));
        auto& childHierarchy = inRegistry.Get<Hierarchy>(inChild);
        auto& parentHierarchy = inRegistry.Get<Hierarchy>(childHierarchy.parent);

        if (parentHierarchy.firstChild == inChild) {
            Assert(childHierarchy.prevBro == entityNull);
            parentHierarchy.firstChild = childHierarchy.nextBro;
            if (childHierarchy.nextBro != entityNull) {
                auto& nextBroHierarchy = inRegistry.Get<Hierarchy>(childHierarchy.nextBro);
                nextBroHierarchy.prevBro = entityNull;
            }
            childHierarchy.nextBro = entityNull;
        } else {
            Assert(childHierarchy.prevBro != entityNull);
            const auto prevBro = childHierarchy.prevBro;
            const auto nextBro = childHierarchy.nextBro;
            auto& prevBroHierarchy = inRegistry.Get<Hierarchy>(prevBro);
            Assert(prevBroHierarchy.nextBro == inChild);
            prevBroHierarchy.nextBro = nextBro;
            if (nextBro != entityNull) {
                auto& nextBroHierarchy = inRegistry.Get<Hierarchy>(nextBro);
                Assert(nextBroHierarchy.prevBro == inChild);
                nextBroHierarchy.prevBro = prevBro;
            }
            childHierarchy.prevBro = entityNull;
            childHierarchy.nextBro = entityNull;
        }
        childHierarchy.parent = entityNull;
    }

    void HierarchyOps::TraverseChildren(ECRegistry& inRegistry, Entity inParent, const TraverseFunc& inFunc)
    {
        const auto& parentHierarchy = inRegistry.Get<Hierarchy>(inParent);
        for (auto child = parentHierarchy.firstChild; child != entityNull;) {
            const auto& childHierarchy = inRegistry.Get<Hierarchy>(child);
            inFunc(child, inParent);
            child = childHierarchy.nextBro;
        }
    }

    void HierarchyOps::TraverseChildrenRecursively(ECRegistry& inRegistry, Entity inParent, const TraverseFunc& inFunc) // NOLINT
    {
        const auto& parentHierarchy = inRegistry.Get<Hierarchy>(inParent);
        for (auto child = parentHierarchy.firstChild; child != entityNull;) {
            const auto& childHierarchy = inRegistry.Get<Hierarchy>(child);
            inFunc(child, inParent);
            TraverseChildrenRecursively(inRegistry, child, inFunc);
            child = childHierarchy.nextBro;
        }
    }
}
