//
// Created by Zach Lee on 2021/9/15.
//

#ifndef EXPLOSION_HIERARCHYPROXY_H
#define EXPLOSION_HIERARCHYPROXY_H

#include <Engine/ECS.h>

namespace Explosion {
    namespace ECS {
        class Registry;
    }

    class HierarchyProxy {
    public:
        HierarchyProxy(ECS::Registry& reg);
        ~HierarchyProxy() = default;

        void SetParent(ECS::Entity entity, ECS::Entity parent);

        ECS::Entity GetParent(ECS::Entity entity) const;

        ECS::Entity GetFirstChild(ECS::Entity entity) const;

        ECS::Entity GetPrevSibling(ECS::Entity entity) const;

        ECS::Entity GetNextSibling(ECS::Entity entity) const;

        std::vector<ECS::Entity> GetChildren(ECS::Entity entity) const;

        void Tick(ECS::Registry& registry, float time);
    private:
        ECS::Registry& registry;
    };

}

#endif//EXPLOSION_HIERARCHYSYSTEM_H
