//
// Created by LiZhen on 2021/7/21.
//

#ifndef EXPLOSION_REFLECTION_H
#define EXPLOSION_REFLECTION_H

#include <entt/entt.hpp>

namespace Explosion {

    template <typename Type, typename...Spec>
    using ReflFactory = entt::meta_factory<Type, Spec...>;
    using ReflType = entt::meta_type;
    using ReflIdType = entt::id_type;
    using ReflAny = entt::meta_any;

    class Reflection {
    public:
        Reflection() = default;
        ~Reflection() = default;

        template <typename Type>
        static auto Factory() noexcept
        {
            return entt::meta<Type>();
        }

        template <typename Type>
        static auto Resolve() noexcept
        {
            return entt::resolve<Type>();
        }

        static auto Resolve(const ReflIdType id)
        {
            return entt::resolve(id);
        }
    };
}

#endif // EXPLOSION_REFLECTION_H
