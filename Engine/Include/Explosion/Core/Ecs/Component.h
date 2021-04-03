//
// Created by Administrator on 2021/4/3 0003.
//

#ifndef EXPLOSION_COMPONENT_H
#define EXPLOSION_COMPONENT_H

#include <Explosion/Core/Ecs/Wrapper.h>

namespace Explosion {
    using CompType = size_t;

    struct Component {};

    template <typename... Args>
    class CompView {
    public:
        CompView(entt::view<Args...>& view) : view(view) {}

        ~CompView() {}

        template <typename Func>
        void Each(const Func& func)
        {
            view.each(func);
        }

    private:
        entt::view<Args...> view;
    };
}

#endif //EXPLOSION_COMPONENT_H
