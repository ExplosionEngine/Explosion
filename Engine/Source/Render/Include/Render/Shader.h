//
// Created by johnk on 2022/7/24.
//

#pragma once

#include <vector>
#include <unordered_map>
#include <utility>

#include <Common/Utility.h>
#include <Common/Debug.h>
#include <Common/Hash.h>
#include <Common/File.h>
#include <Common/Path.h>

namespace Render {
    class Shader {};

    using VariantKey = uint64_t;
    using ShaderByteCode = std::vector<uint8_t>;

    class IShaderType {
        virtual std::string GetName() = 0;
        virtual std::string GetCode(const Common::PathMapper& pathMapper) = 0;
        virtual std::vector<VariantKey> GetVariants() = 0;
        virtual std::vector<std::string> GetDefinitions(VariantKey variantKey) = 0;
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

    template <typename Shader>
    class GlobalShaderType : public IShaderType {
    public:
        static GlobalShaderType& Get()
        {
            static GlobalShaderType instance;
            return instance;
        }

        GlobalShaderType()
        {
            ComputeVariantDefinitions();
        }

        ~GlobalShaderType() = default;
        NON_COPYABLE(GlobalShaderType)

        std::string GetName() override
        {
            return Shader::name;
        }

        std::string GetCode(const Common::PathMapper& pathMapper) override
        {
            return Common::FileUtils::ReadTextFile(pathMapper.Map(Shader::sourceFile));
        }

        std::vector<VariantKey> GetVariants() override
        {
            std::vector<VariantKey> result;
            {
                result.reserve(variantDefinitions.size());
                for (const auto& iter : variantDefinitions) {
                    result.emplace_back(iter.first);
                }
            }
            return result;
        }

        std::vector<std::string> GetDefinitions(VariantKey variantKey) override
        {
            auto iter = variantDefinitions.find(variantKey);
            Assert(iter != variantDefinitions.end());
            return iter->second;
        }

    private:
        void ComputeVariantDefinitions()
        {
            // TODO
        }

        std::unordered_map<VariantKey, std::vector<std::string>> variantDefinitions;
    };

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

    struct StaticBoolShaderVariantFieldImpl {
        using ValueType = bool;

        static std::pair<uint32_t, uint32_t> ValueRange()
        {
            return { 0, 1 };
        }

        void CheckValue() {}

        ValueType value;
    };

    template <uint32_t From, uint32_t To>
    struct StaticRangedIntShaderVariantFieldImpl {
        using ValueType = uint32_t;

        static std::pair<uint32_t, uint32_t> ValueRange()
        {
            return { From, To };
        }

        void CheckValue()
        {
            Assert(From <= value && value <= To);
        }

        ValueType value;
    };

    template <typename... Variants>
    class StaticShaderVariantSetImpl {
    public:
        StaticShaderVariantSetImpl() = default;
        ~StaticShaderVariantSetImpl() = default;
        StaticShaderVariantSetImpl(StaticShaderVariantSetImpl&& other) noexcept : variants(std::move(other)) {}

        static uint32_t VariantNum()
        {
            uint32_t result = 1;
            std::initializer_list<int> { ([&result]() -> void {
                auto valueRange = Variants::ValueRange();
                Assert(valueRange.first <= valueRange.second);
                result *= valueRange.second - valueRange.first + 1;
            }(), 0)... };
            return result;
        }

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

#define GlobalShaderInfo(inName, inSourceFile) \
    static constexpr const char* name = inName; \
    static constexpr const char* sourceFile = inSourceFile; \

#define DefaultStaticVariantFilter \
    static bool StaticVariantFilter(const VariantSet& variantSet) { return true; } \

#define StaticBoolShaderVariantField(inClass, inMacro) \
    struct inClass : public Render::StaticBoolShaderVariantFieldImpl { \
        static constexpr const char* name = #inClass; \
        static constexpr const char* macro = inMacro; \
    }; \

#define StaticRangedIntShaderVariantField(inClass, inMacro, inRangeFrom, inRangeTo) \
    struct inClass : public Render::StaticRangedIntShaderVariantFieldImpl<inRangeFrom, inRangeTo> { \
        static constexpr const char* name = #inClass; \
        static constexpr const char* macro = inMacro; \
    }

#define StaticVariantSet(...) \
    class VariantSet : public Render::StaticShaderVariantSetImpl<__VA_ARGS__> {};

#define RegisterGlobalShader(inClass) \
    static uint8_t _globalShaderRegister_inClass = []() -> uint8_t { \
        Render::GlobalShaderRegistry::Get().Register<inClass>(); \
        return 0; \
    }(); \

namespace Render {
    class MaterialShader : public Shader {};
}
