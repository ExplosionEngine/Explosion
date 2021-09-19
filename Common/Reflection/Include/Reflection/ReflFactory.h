//
// Created by Zach Lee on 2021/7/22.
//

#ifndef EXPLOSION_REFLFACTORY_H
#define EXPLOSION_REFLFACTORY_H

#include <unordered_map>
#include <entt/entt.hpp>
#include <Common/Exception.h>

namespace Explosion {

    using ReflType = entt::meta_type;
    using ReflIdType = entt::id_type;
    using ReflAny = entt::meta_any;

    using ReflNameMap = std::unordered_map<ReflIdType, std::string_view>;

    struct ReflTypeNameMap {
        static ReflNameMap& TypeMap() {
            static ReflNameMap map;
            return map;
        }

        static ReflNameMap& DataMap(ReflIdType id)
        {
            static std::unordered_map<ReflIdType, ReflNameMap> dataMap;
            return dataMap[id];
        }

        static const std::string_view& TypeName(ReflIdType id)
        {
            return TypeMap()[id];
        }

        static const std::string_view& DataName(ReflIdType typeId, ReflIdType dataId)
        {
            return DataMap(typeId)[dataId];
        }
    };

    template<typename Type>
    struct ReflDataNameMap {
        static ReflNameMap& Map() {
            static ReflNameMap map;
            return map;
        }
    };

    template <typename...>
    class ReflFactory;

    template<typename Type, typename... Spec>
    class ReflFactory<Type, Spec...> : public entt::meta_factory<Type, Spec...> {
    public:
        using Base = entt::meta_factory<Type, Spec...>;
        template <typename T, typename... S>
        ReflFactory(const entt::meta_factory<T, S...>& metaFactory)
            : entt::meta_factory<Type, S...>(metaFactory)
        {
        }

        auto type(const std::string_view& name)
        {
            auto id = entt::hashed_string{name.data()};
            Save(ReflTypeNameMap::TypeMap(), id, name);
            return ReflFactory<Type, Type>(Base::type(id));
        }

        template<typename... Args>
        auto ctor() noexcept
        {
            return ReflFactory<Type, Type(Args...)>(Base::template ctor<Args...>());
        }

        template<auto Setter, auto Getter, typename Policy = entt::as_is_t>
        auto data(const std::string_view& name) noexcept {
            auto id = entt::hashed_string{name.data()};
            Save(ReflTypeNameMap::DataMap(entt::resolve<Type>().id()), id, name);
            return ReflFactory<Type, std::integral_constant<decltype(Setter), Setter>,
                std::integral_constant<decltype(Getter), Getter>>(Base::template data<Setter, Getter, Policy>(id));
        }

        template<auto Data, typename Policy = entt::as_is_t>
        auto data(const std::string_view& name) noexcept {
            if constexpr(std::is_member_object_pointer_v<decltype(Data)>) {
                return data<Data, Data, Policy>(name);
            } else {
                auto id = entt::hashed_string{name.data()};
                Save(ReflTypeNameMap::DataMap(entt::resolve<Type>().id()), id, name);
                return ReflFactory<Type, std::integral_constant<decltype(Data), Data>>(Base::template data<Data, Policy>(id));
            }
        }

    private:
        inline void Save(ReflNameMap& map, ReflIdType id, const std::string_view& name)
        {
            auto iter = map.try_emplace(id, name);
            EXPLOSION_ASSERT(iter.second, "conflict id type");
        }
    };

}

#endif // EXPLOSION_REFLFACTORY_H
