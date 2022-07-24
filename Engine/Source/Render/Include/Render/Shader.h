//
// Created by johnk on 2022/7/24.
//

#pragma once

#include <vector>
#include <unordered_map>

#include <Common/Utility.h>
#include <Common/Debug.h>
#include <Common/Hash.h>

namespace Render {
    class Shader {};

    using VariantKey = uint64_t;
    using ShaderByteCode = std::vector<uint8_t>;

    class IShaderType {
        virtual std::string GetName() = 0;
        virtual std::string GetCode() = 0;
        virtual std::vector<VariantKey> GetVariants() = 0;
        virtual std::vector<std::string> GetDefinitions(VariantKey variantKey) = 0;
        virtual std::string GetVariantName(VariantKey variantKey) = 0;
    };

    template <typename Shader>
    class GlobalShaderType : IShaderType {
    public:
        static GlobalShaderType& Get()
        {
            GlobalShaderType instance;
            return instance;
        }

        GlobalShaderType() = default;
        ~GlobalShaderType() = default;
        NON_COPYABLE(GlobalShaderType)

        std::string GetName() override
        {
            // TODO
        }

        std::string GetCode() override
        {
            // TODO
        }

        std::vector<VariantKey> GetVariants() override
        {
            // TODO
        }

        std::vector<std::string> GetDefinitions(VariantKey variantKey) override
        {
            // TODO
        }

        std::string GetVariantName(VariantKey variantKey) override
        {
            // TODO
        }
    };

    class ShaderByteCodeStorage {
    public:
        static ShaderByteCodeStorage& Get();
        ShaderByteCodeStorage();
        ~ShaderByteCodeStorage();
        NON_COPYABLE(ShaderByteCodeStorage)

        void EmplaceByteCodePackage(IShaderType* shaderTypeKey, std::unordered_map<VariantKey, ShaderByteCode>&& byteCodePackage);
        const std::unordered_map<VariantKey, ShaderByteCode>& GetByteCodePackage(IShaderType* shaderTypeKey);

    private:
        std::unordered_map<IShaderType*, std::unordered_map<VariantKey, ShaderByteCode>> byteCodePackages;
    };
}

namespace Render {
    class GlobalShader : public Shader {};

    template <typename T>
    class GlobalShaderMap {
    public:
        GlobalShaderMap() = default;
        ~GlobalShaderMap() = default;
        NON_COPYABLE(GlobalShaderMap)

        [[nodiscard]] const ShaderByteCode& GetByteCode(const typename T::VariantSet& variantSet) const
        {
            auto iter = byteCodePackage.find(variantSet.Hash());
            Assert(iter != byteCodePackage.end());
            return iter->second();
        }

    private:
        const std::unordered_map<VariantKey, ShaderByteCode>& byteCodePackage;
    };

    class GlobalShaderRegistry {
    public:
        static GlobalShaderRegistry& Get()
        {
            static GlobalShaderRegistry instance;
            return instance;
        }

        GlobalShaderRegistry() = default;
        ~GlobalShaderRegistry() = default;
        NON_COPYABLE(GlobalShaderRegistry)

        template <typename Shader>
        void Register()
        {
            pendingShaderTypes.emplace_back(&GlobalShaderType<Shader>::Get());
        }

        std::vector<IShaderType*>&& GetPendingShaderType()
        {
            return std::move(pendingShaderTypes);
        }

    private:
        std::vector<IShaderType*> pendingShaderTypes;
    };

    struct StaticBoolShaderVariantImpl {
        using ValueType = bool;

        void CheckValue() {}

        ValueType value;
    };

    template <uint32_t From, uint32_t To>
    struct StaticRangedIntShaderVariantImpl {
        using ValueType = uint32_t;

        void CheckValue()
        {
            Assert(From <= value && value <= To);
        }

        ValueType value;
    };

    template <typename... Variants>
    class StaticShaderVariantSet {
    public:
        StaticShaderVariantSet() = default;
        ~StaticShaderVariantSet() = default;
        StaticShaderVariantSet(StaticShaderVariantSet&& other) noexcept : variants(std::move(other)) {}

        template <typename Variant>
        void Set(typename Variant::ValueType value)
        {
            std::get<Variant>(variants).value = value;
            std::get<Variant>(variants).CheckValue();
        }

        template <typename Variant>
        typename Variant::ValueType Get()
        {
            return std::get<Variant>(variants).value;
        }

        VariantKey Hash()
        {
            return Common::HashUtils::CityHash(&variants, sizeof(std::tuple<Variants...>));
        }

    private:
        std::tuple<Variants...> variants;
    };
}

#define StaticBoolShaderVariant(inMacro) \
    StaticBoolShaderVariantImpl {     \
        static constexpr std::string_view macro = inMacro; \
    };

#define StaticRangedIntShaderVariant(inMacro, inRangeFrom, inRangeTo) \
    StaticRangedIntShaderVariantImpl {                    \
        static constexpr std::string_view macro = inMacro; \
        static constexpr uint32_t rangeFrom = inRangeFrom; \
        static constexpr uint32_t rangeTo = inRangeTo; \
    }

namespace Render {
    class MaterialShader : public Shader {};
}
