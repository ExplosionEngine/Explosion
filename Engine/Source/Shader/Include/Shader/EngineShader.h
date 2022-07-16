//
// Created by johnk on 2022/7/13.
//

#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <unordered_map>

#include <Common/Hash.h>
#include <Common/Debug.h>

#define BoolVariant(macroName) \
    Shader::BoolVariantDef { \
    public: \
        static constexpr std::string_view macro = macroName; \
    } \

#define RangedIntVariant(macroName, From, To) \
    Shader::RangedIntVariantDef<From, To> { \
    public: \
        static constexpr std::string_view macro = macroName; \
    } \

namespace Shader {
    struct BoolVariantDef {
        using ValueType = bool;

        static void CheckValue(ValueType inValue) {}

        ValueType value;
    };

    template <uint8_t F, uint8_t T>
    struct RangedIntVariantDef {
        using ValueType = uint8_t;
        static constexpr uint8_t RangeFrom = F;
        static constexpr uint8_t RangeTo = T;

        static void CheckValue(ValueType inValue)
        {
            Assert(inValue >= RangeFrom && inValue <= RangeTo);
        }

        ValueType value;
    };

    template <typename... T>
    struct VariantSet {
    public:
        template <typename V>
        void Set(typename V::ValueType value)
        {
            V::CheckValue(value);
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
    class EngineShader {};

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

        template <typename F>
        void ForEachVariant(F&& func)
        {
            // TODO
        }

    private:
        EngineShaderMap() = default;

        std::unordered_map<uint64_t, std::vector<uint8_t>> byteCodes;
    };
}
