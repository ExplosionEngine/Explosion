//
// Created by johnk on 2023/4/4.
//

#include <Runtime/Component/Transform.h>
#include <Runtime/World.h>

namespace Runtime {
    void TransformComponent::TraverseChildren(const EntityTraverseFunc& func) const
    {
        Entity cur = firstChild;
        while (cur != ENTITY_NULL) {
            func(cur);

            auto* transform = world->GetComponent<TransformComponent>(cur);
            Assert(transform);
            cur = transform->nextBrother;
        }
    }

    void TransformComponent::TraverseOffspring(const EntityTraverseFunc& func) const // NOLINT
    {
        std::vector<Entity> pendings;
        TraverseChildren([&](Entity child) -> void {
            func(child);
            pendings.emplace_back(child);
        });
        for (auto pending : pendings) {
            auto* transform = world->GetComponent<TransformComponent>(pending);
            Assert(transform);
            transform->TraverseOffspring(func);
        }
    }

    void TransformComponent::TraverseBrothers(const Runtime::EntityTraverseFunc& func) const
    {
        Entity cur = nextBrother;
        while (cur != ENTITY_NULL) {
            func(cur);

            auto* transform = world->GetComponent<TransformComponent>(cur);
            Assert(transform);
            cur = transform->nextBrother;
        }
    }
}
