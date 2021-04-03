//
// Created by Administrator on 2021/4/3 0003.
//

#ifndef EXPLOSION_COMPONENT_H
#define EXPLOSION_COMPONENT_H

#include <Explosion/Core/Ecs/Wrapper.h>

namespace Explosion {
    struct Component {};

    template <typename... Args>
    using CompView = entt::basic_view<Args...>;
}

#endif //EXPLOSION_COMPONENT_H
