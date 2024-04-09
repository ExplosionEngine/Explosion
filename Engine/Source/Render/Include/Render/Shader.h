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
#include <Common/Math/Vector.h>
#include <RHI/Common.h>
#include <RHI/Device.h>
#include <RHI/ShaderModule.h>
#include <RHI/BindGroupLayout.h>
#include <Core/Paths.h>

namespace Render {
    class Shader {};

    using ShaderTypeKey = uint64_t;
    using VariantKey = uint64_t;
    using ShaderByteCode = std::vector<uint8_t>;
    using ShaderStage = RHI::ShaderStageBits;

    class IShaderType {
        virtual std::string GetName() = 0;
        virtual ShaderTypeKey GetHash() = 0;
        virtual std::string GetCode() = 0;
        virtual std::vector<VariantKey> GetVariants() = 0;
        virtual std::vector<std::string> GetDefinitions(VariantKey variantKey) = 0;
    };

    struct ShaderReflectionData {
        using LayoutIndex = uint8_t;

        std::unordered_map<std::string, std::pair<LayoutIndex, RHI::ResourceBinding>> resourceBindings;
    };

    struct ShaderInstance {
        RHI::ShaderModule* rhiHandle = nullptr;
        ShaderTypeKey typeKey = 0;
        VariantKey variantKey = 0;
        ShaderReflectionData reflectionData;

        bool IsValid() const
        {
            return rhiHandle != nullptr;
        }

        size_t Hash() const
        {
            if (!IsValid()) {
                return 0;
            }

            std::vector<size_t> values = {
                typeKey,
                variantKey
            };
            return Common::HashUtils::CityHash(values.data(), values.size() * sizeof(size_t));
        }
    };

    class ShaderByteCodeStorage {
    public:
        static ShaderByteCodeStorage& Get();
        ShaderByteCodeStorage();
        ~ShaderByteCodeStorage();
        NonCopyable(ShaderByteCodeStorage)

        // TODO fill byte codes after compiling using this interface
        void UpdateByteCodePackage(IShaderType* shaderTypeKey, std::unordered_map<VariantKey, ShaderByteCode>&& byteCodePackage);
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
        NonCopyable(GlobalShaderType)

        std::string GetName() override
        {
            return Shader::name;
        }

        ShaderTypeKey GetHash() override
        {
            return Common::HashUtils::CityHash(Shader::name, sizeof(Shader::name));
        }

        std::string GetCode() override
        {
            static std::unordered_map<std::string, std::string> pathMap = {
                { "/Engine/Shader", Core::Paths::EngineShaderPath().string() }
            };

            std::string sourceFile = Shader::sourceFile;
            for (const auto& iter : pathMap) {
                if (sourceFile.starts_with(iter.first)) {
                    return Common::FileUtils::ReadTextFile(Common::StringUtils::Replace(sourceFile, iter.first, iter.second));
                }
            }
            QuickFail();
            return "";
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
        static GlobalShaderMap& Get(RHI::Device& device)
        {
            static std::unordered_map<RHI::Device*, Common::UniqueRef<GlobalShaderMap<T>>> map;
            auto iter = map.find(&device);
            if (iter == map.end()) {
                map[&device] = Common::UniqueRef<GlobalShaderMap<T>>(new GlobalShaderMap<T>(device));
            }
            return *map[&device];
        }

        ~GlobalShaderMap() = default;
        NonCopyable(GlobalShaderMap)

        void Invalidate()
        {
            auto variantNum = T::VariantSet::VariantNum();
            shaderModules.clear();
            shaderModules.reserve(variantNum);
        }

        ShaderInstance GetShaderInstance(const typename T::VariantSet& variantSet)
        {
            auto variantKey = variantSet.Hash();
            auto iter = shaderModules.find(variantKey);
            if (iter != shaderModules.end()) {
                const auto& shaderByteCode = GetByteCode(variantSet);
                shaderModules[variantKey] = device.CreateShaderModule(RHI::ShaderModuleCreateInfo(T::name, shaderByteCode));
            }

            ShaderInstance result;
            result.rhiHandle = shaderModules[variantKey].Get();
            result.typeKey = GlobalShaderType<T>::Get().GetHash();
            result.variantKey = variantKey;
            return result;
        }

    private:
        explicit GlobalShaderMap(RHI::Device& inDevice)
            : device(inDevice)
        {
            auto variantNum = T::VariantSet::VariantNum();
            shaderModules.reserve(variantNum);
        }

        [[nodiscard]] const ShaderByteCode& GetByteCode(const typename T::VariantSet& variantSet) const
        {
            const auto& byteCodePackage = ShaderByteCodeStorage::Get().GetByteCodePackage(&GlobalShaderType<T>::Get());

            auto iter = byteCodePackage.find(variantSet.Hash());
            Assert(iter != byteCodePackage.end());
            return iter->second;
        }

        RHI::Device& device;
        std::unordered_map<VariantKey, Common::UniqueRef<RHI::ShaderModule>> shaderModules;
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
        NonCopyable(GlobalShaderRegistry)

        template <typename Shader>
        void Register()
        {
            shaderTypes.emplace_back(&GlobalShaderType<Shader>::Get());
        }

        std::vector<IShaderType*>&& GetShaderTypes()
        {
            return std::move(shaderTypes);
        }

    private:
        std::vector<IShaderType*> shaderTypes;
    };

    class BoolShaderVariantFieldImpl {
    public:
        using ValueType = bool;

        static constexpr std::pair<uint32_t, uint32_t> valueRange = { 0, 1 };
        static constexpr ValueType defaultValue = false;

        BoolShaderVariantFieldImpl() : value(static_cast<uint32_t>(defaultValue)) {}
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
            (void) std::initializer_list<int> { ([this]() -> void { std::get<Variants>(variants).Set(Variants::defaultValue); }(), 0)... };
        }

        VariantSetImpl(const VariantSetImpl& other)
        {
            (void) std::initializer_list<int> { ([this, &other]() -> void { std::get<Variants>(variants).Set(std::get<Variants>(other.variants).Get()); }(), 0)... };
        }

        VariantSetImpl(VariantSetImpl&& other) noexcept : variants(std::move(other.variants)) {}
        ~VariantSetImpl() = default;

        static uint32_t VariantNum()
        {
            uint32_t result = 1;
            (void) std::initializer_list<int> { ([&result]() -> void {
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

            (void) std::initializer_list<int> { ([&variantSets]() -> void {
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
            (void) std::initializer_list<int> { ([&result, this]() -> void {
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

#define ShaderInfo(inName, inSourceFile, inEntryPoint, inStage) \
    static constexpr const char* name = inName; \
    static constexpr const char* sourceFile = inSourceFile; \
    static constexpr const char* entryPoint = inEntryPoint; \
    static constexpr Render::ShaderStage stage = inStage; \

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
