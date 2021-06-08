//
// Created by John Kindem on 2021/6/8.
//

#ifndef EXPLOSION_VIEW_H
#define EXPLOSION_VIEW_H

#include <Explosion/World/ECS/Entity.h>

namespace Explosion::ECS {
    class Registry;

    template <typename... T>
    using ExcludeT = entt::exclude_t<T...>;

    template <typename... T>
    class View {
    public:
        ~View() = default;

        template <typename Func>
        void Each(const Func& func)
        {
            return view.each(func);
        }

    private:
        friend Registry;

        explicit View(entt::view<T...> view) : view(std::move(view)) {}

        entt::view<T...> view;
    };
}

#endif //EXPLOSION_VIEW_H
