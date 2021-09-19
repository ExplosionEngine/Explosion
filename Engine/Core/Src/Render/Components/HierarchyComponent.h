//
// Created by Zach Lee on 2021/9/19.
//

#ifndef EXPLOSION_HIERARCHYCOMPONENT_H
#define EXPLOSION_HIERARCHYCOMPONENT_H

#include <Engine/ECS.h>

namespace Explosion {

    struct HierarchyComponent {
        ECS::Entity first  = ECS::INVALID_ENTITY;
        ECS::Entity next   = ECS::INVALID_ENTITY;
        ECS::Entity prev   = ECS::INVALID_ENTITY;
        ECS::Entity parent = ECS::INVALID_ENTITY;
        bool refreshFlag = false;
    };

}

#endif//EXPLOSION_HIERARCHYCOMPONENT_H
