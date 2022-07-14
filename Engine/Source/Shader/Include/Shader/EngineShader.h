//
// Created by johnk on 2022/7/13.
//

#pragma once

#include <string>
#include <vector>
#include <tuple>

#include <Common/Hash.h>

#define BoolVariant(macroName) \
    : public Shader::BoolVariantDef { \
    public: \
        static constexpr std::string_view macro = macroName; \
    } \

#define RangedIntVariant(macroName, From, To) \
    : public Shader::RangedIntVariantDef<From, To> { \
    public: \
        static constexpr std::string_view macro = macroName; \
    } \

namespace Shader {
    struct BoolVariantDef {
        using ValueType = bool;

        ValueType value;
    };

    template <uint8_t F, uint8_t T>
    struct RangedIntVariantDef {
        using ValueType = uint8_t;
        static constexpr uint8_t RangeFrom = F;
        static constexpr uint8_t RagenTo = T;

        ValueType value;
    };

    template <typename... T>
    struct VariantSet {
    public:
        template <typename V>
        void Set(typename V::ValueType value)
        {
            std::get<V>(values).value = value;
        }

        template <typename V>
        typename V::ValueType Get()
        {
            return std::get<V>(values).value;
        }

        uint64_t Hash()
        {
            return Common::HashUtils::CityHash(&values, sizeof(std::tuple<T...>));
        }

    private:
        std::tuple<T...> values;
    };
}

namespace Shader {
    struct EngineShader {};

    template <typename T>
    class EngineShaderMap {
    public:
        using VariantSet = typename T::VariantSet;

        static EngineShaderMap& Get()
        {
            static EngineShaderMap instance;
            return instance;
        }

        ~EngineShaderMap() = default;

        std::vector<uint8_t>& GetByteCode(const VariantSet& variantSet)
        {
            // TODO
        }

    private:
        EngineShaderMap() = default;
    };
}
