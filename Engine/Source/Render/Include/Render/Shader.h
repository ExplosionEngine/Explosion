//
// Created by johnk on 2022/7/24.
//

#pragma once

#include <vector>
#include <unordered_map>
#include <utility>
#include <tuple>
#include <functional>

#include <Common/Utility.h>
#include <Common/Debug.h>
#include <Common/Hash.h>
#include <Common/File.h>
#include <Common/Path.h>
#include <Common/Math/Vector2.h>
#include <Common/Math/Vector3.h>
#include <Common/Math/Vector4.h>
#include <Common/Math/Matrix4.h>

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
            variantDefinitions.reserve(Shader::VariantSet::VariantNum());
            Shader::VariantSet::TraverseAll([this](auto&& variantSetImpl) -> void {
                const auto* variantSet = static_cast<typename Shader::VariantSet*>(&variantSetImpl);
                if (!Shader::VariantFilter(*variantSet)) {
                    return;
                }
                variantDefinitions[variantSet->Hash()] = variantSet->ComputeDefinitions();
            });
        }

        std::unordered_map<VariantKey, std::vector<std::string>> variantDefinitions;
    };

    template <typename T>
    class GlobalShaderMap {
    public:
        GlobalShaderMap() : byteCodePackage(ShaderByteCodeStorage::Get().GetByteCodePackage(&GlobalShaderType<T>::Get)) {}

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

    class BoolShaderVariantFieldImpl {
    public:
        using ValueType = bool;

        static constexpr std::pair<uint32_t, uint32_t> valueRange = { 0, 1 };
        static constexpr ValueType defaultValue = false;

        BoolShaderVariantFieldImpl() : value(defaultValue) {}
        BoolShaderVariantFieldImpl(BoolShaderVariantFieldImpl&& other) noexcept : value(other.value) {}
        ~BoolShaderVariantFieldImpl() = default;

        void Set(ValueType inValue)
        {
            value = inValue ? 1 : 0;
        }

        [[nodiscard]] ValueType Get() const
        {
            return value == 1;
        }

        [[nodiscard]] uint32_t GetNumberValue() const
        {
            return value;
        }

    private:
        uint32_t value;
    };

    template <uint32_t From, uint32_t To>
    struct RangedIntShaderVariantFieldImpl {
    public:
        using ValueType = uint32_t;

        static constexpr std::pair<uint32_t, uint32_t> valueRange = { From, To };
        static constexpr ValueType defaultValue = From;

        RangedIntShaderVariantFieldImpl() : value(defaultValue) {}
        RangedIntShaderVariantFieldImpl(RangedIntShaderVariantFieldImpl&& other) noexcept : value(other.value) {}
        ~RangedIntShaderVariantFieldImpl() = default;

        void Set(ValueType inValue)
        {
            Assert(From <= value && value <= To);
            value = inValue;
        }

        [[nodiscard]] ValueType Get() const
        {
            return value;
        }

        [[nodiscard]] uint32_t GetNumberValue() const
        {
            return value;
        }

    private:
        uint32_t value;
    };

    template <typename... Variants>
    class VariantSetImpl {
    public:
        VariantSetImpl()
        {
            std::initializer_list<int> { ([this]() -> void { std::get<Variants>(variants).Set(Variants::defaultValue); }(), 0)... };
        }

        VariantSetImpl(const VariantSetImpl& other)
        {
            std::initializer_list<int> { ([this, &other]() -> void { std::get<Variants>(variants).Set(std::get<Variants>(other.variants).Get()); }(), 0)... };
        }

        VariantSetImpl(VariantSetImpl&& other) noexcept : variants(std::move(other.variants)) {}
        ~VariantSetImpl() = default;

        static uint32_t VariantNum()
        {
            uint32_t result = 1;
            std::initializer_list<int> { ([&result]() -> void {
                auto valueRange = Variants::valueRange;
                Assert(valueRange.first <= valueRange.second);
                result *= valueRange.second - valueRange.first + 1;
            }(), 0)... };
            return result;
        }

        template <typename F>
        static void TraverseAll(F&& func)
        {
            std::vector<VariantSetImpl<Variants...>> variantSets;
            variantSets.reserve(VariantNum());
            variantSets.emplace_back(VariantSetImpl<Variants...>());

            std::initializer_list<int> { ([&variantSets]() -> void {
                auto valueRange = Variants::valueRange;
                auto variantSetsSize = variantSets.size();
                for (auto i = valueRange.first; i <= valueRange.second; i++) {
                    if (i == valueRange.first) {
                        for (auto j = 0; j < variantSetsSize; j++) {
                            variantSets[j].template Set<Variants>(static_cast<typename Variants::ValueType>(i));
                        }
                    } else {
                        for (auto j = 0; j < variantSetsSize; j++) {
                            variantSets.emplace_back(variantSets[j]);
                            variantSets.back().template Set<Variants>(static_cast<typename Variants::ValueType>(i));
                        }
                    }
                }
            }(), 0)... };

            for (auto& variantSet : variantSets) {
                func(variantSet);
            }
        }

        template <typename Variant>
        void Set(typename Variant::ValueType value)
        {
            std::get<Variant>(variants).Set(value);
        }

        template <typename Variant>
        typename Variant::ValueType Get()
        {
            return std::get<Variant>(variants).Get();
        }

        [[nodiscard]] std::vector<std::string> ComputeDefinitions() const
        {
            std::vector<std::string> result;
            result.reserve(sizeof...(Variants));
            std::initializer_list<int> { ([&result, this]() -> void {
                result.emplace_back(std::string(Variants::macro) + "=" + std::to_string(std::get<Variants>(variants).GetNumberValue()));
            }(), 0)... };
            return result;
        }

        [[nodiscard]] VariantKey Hash() const
        {
            return Common::HashUtils::CityHash(&variants, sizeof(std::tuple<Variants...>));
        }

    private:
        std::tuple<Variants...> variants;
    };
}

#define ShaderInfo(inName, inSourceFile) \
    static constexpr const char* name = inName; \
    static constexpr const char* sourceFile = inSourceFile; \

#define DefaultVariantFilter \
    static bool VariantFilter(const VariantSet& variantSet) { return true; } \

#define BoolShaderVariantField(inClass, inMacro) \
    struct inClass : public Render::BoolShaderVariantFieldImpl { \
        static constexpr const char* name = #inClass; \
        static constexpr const char* macro = inMacro; \
    }; \

#define RangedIntShaderVariantField(inClass, inMacro, inRangeFrom, inRangeTo) \
    struct inClass : public Render::RangedIntShaderVariantFieldImpl<inRangeFrom, inRangeTo> { \
        static constexpr const char* name = #inClass; \
        static constexpr const char* macro = inMacro; \
    }

#define VariantSet(...) \
    class VariantSet : public Render::VariantSetImpl<__VA_ARGS__> {};

#define RegisterGlobalShader(inClass) \
    static uint8_t _globalShaderRegister_inClass = []() -> uint8_t { \
        Render::GlobalShaderRegistry::Get().Register<inClass>(); \
        return 0; \
    }(); \

namespace Render {
    class MaterialShader : public Shader {};
}
