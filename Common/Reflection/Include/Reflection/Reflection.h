//
// Created by Zach Lee on 2021/7/21.
//

#ifndef EXPLOSION_REFLECTION_H
#define EXPLOSION_REFLECTION_H

#include <entt/entt.hpp>
#include <Reflection/ReflFactory.h>

namespace Explosion {

    class Reflection {
    public:
        Reflection() = default;
        ~Reflection() = default;

        template <typename Type>
        static auto Factory() noexcept
        {
            return ReflFactory<Type, Type>(entt::meta<Type>());
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

        static auto Resolve(const std::string_view& name)
        {
            return Resolve(entt::hashed_string{name.data()});
        }

      private:
        static std::unordered_map<ReflIdType, std::string_view> idMap;
    };
}

#endif // EXPLOSION_REFLECTION_H
