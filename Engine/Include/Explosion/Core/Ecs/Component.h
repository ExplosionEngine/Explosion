//
// Created by Administrator on 2021/4/2 0002.
//

#ifndef EXPLOSION_COMPONENT_H
#define EXPLOSION_COMPONENT_H

#include <entt/entt.hpp>

namespace Explosion {
    struct ComponentBase {};

    template <typename... Args>
    using CompView = entt::view<Args...>;
}

#endif //EXPLOSION_COMPONENT_H
