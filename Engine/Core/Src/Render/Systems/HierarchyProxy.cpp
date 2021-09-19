//
// Created by Zach Lee on 2021/9/19.
//

#include <Engine/ECS.h>
#include <Engine/Render/Systems/HierarchyProxy.h>
#include "../Components/HierarchyComponent.h"

namespace Explosion {

    static void TryAdd(ECS::Registry& registry, ECS::Entity entity)
    {
        auto comp = registry.GetComponent<HierarchyComponent>(entity);
        if (comp == nullptr) {
            registry.AddComponent<HierarchyComponent>(entity, HierarchyComponent{});
        }
    }

    template <typename Pred>
    static void HierarchyPred(ECS::Registry& registry, ECS::Entity entity, Pred&& pred)
    {
        if (!registry.IsActive(entity))
            return;

        HierarchyComponent* hp = registry.GetComponent<HierarchyComponent>(entity);
        if (hp == nullptr)
            return;

        pred(hp);
    }

    template <typename Pred>
    static void ForEachChild(ECS::Registry& registry, ECS::Entity entity, Pred&& pred)
    {
        if (!registry.IsActive(entity))
            return;

        HierarchyComponent* hp = registry.GetComponent<HierarchyComponent>(entity);
        if (hp == nullptr)
            return;

        auto child = hp->first;
        while (registry.IsActive(child)) {
            auto cp = registry.GetComponent<HierarchyComponent>(child);
            if (!pred(child, cp)) {
                break;
            }
            child = cp->next;
        }
    }

    static void RemoveFromOldParent(ECS::Registry& registry, ECS::Entity entity, ECS::Entity old)
    {
        if (!registry.IsActive(old))
            return;

        HierarchyComponent* hp = registry.GetComponent<HierarchyComponent>(old);
        HierarchyComponent* ep = registry.GetComponent<HierarchyComponent>(entity);
        HierarchyPred(registry, ep->prev, [ep](HierarchyComponent* comp) {
            comp->next = ep->next;
        });
        HierarchyPred(registry, ep->next, [ep](HierarchyComponent* comp) {
            comp->prev = ep->prev;
        });

        if (hp->first == entity) {
            hp->first = ep->next;
        }

        ep->prev = ECS::INVALID_ENTITY;
        ep->next = ECS::INVALID_ENTITY;
        ep->parent = ECS::INVALID_ENTITY;
    }

    HierarchyProxy::HierarchyProxy(ECS::Registry& reg)
        : registry(reg)
    {
    }

    void HierarchyProxy::SetParent(ECS::Entity entity, ECS::Entity parent)
    {
        if (!registry.IsActive(entity) || !registry.IsActive(parent))
            return;

        TryAdd(registry, entity);
        TryAdd(registry, parent);
        auto eh = registry.GetComponent<HierarchyComponent>(entity);
        auto ph = registry.GetComponent<HierarchyComponent>(parent);
        RemoveFromOldParent(registry, entity, eh->parent);

        auto sibling = ph->first;
        if (registry.IsActive(ph->first)) {
            auto sh = registry.GetComponent<HierarchyComponent>(sibling);
            while (sh != nullptr && registry.IsActive(sh->next)) {
                sibling = sh->next;
                sh = registry.GetComponent<HierarchyComponent>(sibling);
            }
            sh->next = entity;
            eh->prev = sibling;
        } else {
            ph->first = entity;
        }
        eh->parent = parent;
    }

    ECS::Entity HierarchyProxy::GetParent(ECS::Entity entity) const
    {
        ECS::Entity res = ECS::INVALID_ENTITY;
        HierarchyPred(registry, entity, [&res](HierarchyComponent* hp) {
          res = hp->parent;
        });
        return res;
    }

    ECS::Entity HierarchyProxy::GetFirstChild(ECS::Entity entity) const
    {
        ECS::Entity res = ECS::INVALID_ENTITY;
        HierarchyPred(registry, entity, [&res](HierarchyComponent* hp) {
          res = hp->first;
        });
        return res;
    }

    ECS::Entity HierarchyProxy::GetPrevSibling(ECS::Entity entity) const
    {
        ECS::Entity res = ECS::INVALID_ENTITY;
        HierarchyPred(registry, entity, [&res](HierarchyComponent* hp) {
          res = hp->prev;
        });
        return res;
    }

    ECS::Entity HierarchyProxy::GetNextSibling(ECS::Entity entity) const
    {
        ECS::Entity res = ECS::INVALID_ENTITY;
        HierarchyPred(registry, entity, [&res](HierarchyComponent* hp) {
          res = hp->next;
        });
        return res;
    }

    std::vector<ECS::Entity> HierarchyProxy::GetChildren(ECS::Entity entity) const
    {
        if (!registry.IsActive(entity))
            return {};

        HierarchyComponent* hp = registry.GetComponent<HierarchyComponent>(entity);
        if (hp == nullptr)
            return {};

        std::vector<ECS::Entity> res;
        ForEachChild(registry, entity, [&res](ECS::Entity entity, HierarchyComponent*) {
            res.emplace_back(entity);
            return true;
        });
        return res;
    }

}