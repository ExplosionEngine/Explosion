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
#include <RHI/Pipeline.h>
#include <Core/Paths.h>

namespace Render {
    class Shader {};

    using ShaderTypeKey = uint64_t;
    using VariantKey = uint64_t;
    using ShaderStage = RHI::ShaderStageBits;

    struct ShaderReflectionData {
        using VertexSemantic = std::string;
        using ResourceBindingName = std::string;
        using LayoutIndex = uint8_t;
        using LayoutAndResourceBinding = std::pair<LayoutIndex, RHI::ResourceBinding>;

        ShaderReflectionData();
        ShaderReflectionData(const ShaderReflectionData& inOther);
        ShaderReflectionData(ShaderReflectionData&& inOther) noexcept;
        ShaderReflectionData& operator=(const ShaderReflectionData& inOther);

        const RHI::PlatformVertexBinding& QueryVertexBindingChecked(const VertexSemantic& inSemantic) const;
        const LayoutAndResourceBinding& QueryResourceBindingChecked(const ResourceBindingName& inName) const;

        std::unordered_map<VertexSemantic, RHI::PlatformVertexBinding> vertexBindings;
        std::unordered_map<ResourceBindingName, LayoutAndResourceBinding> resourceBindings;
    };

    struct ShaderArchive {
        std::vector<uint8_t> byteCode;
        ShaderReflectionData reflectionData;
    };

    using ShaderArchivePackage = std::unordered_map<VariantKey, ShaderArchive>;

    class IShaderType {
        virtual std::string GetName() = 0;
        virtual ShaderTypeKey GetHash() = 0;
        virtual std::string GetCode() = 0;
        virtual std::vector<VariantKey> GetVariants() = 0;
        virtual std::vector<std::string> GetDefinitions(VariantKey variantKey) = 0;
    };

    struct ShaderInstance {
        RHI::ShaderModule* rhiHandle = nullptr;
        ShaderTypeKey typeKey = 0;
        VariantKey variantKey = 0;
        const ShaderReflectionData* reflectionData;

        bool IsValid() const;
        size_t Hash() const;
    };

    class ShaderArchiveStorage {
    public:
        static ShaderArchiveStorage& Get();
        ShaderArchiveStorage();
        ~ShaderArchiveStorage();
        NonCopyable(ShaderArchiveStorage)

        // TODO fill byte codes after compiling using this interface
        void UpdateShaderArchivePackage(IShaderType* shaderTypeKey, ShaderArchivePackage&& shaderArchivePackage);
        const ShaderArchivePackage& GetShaderArchivePackage(IShaderType* shaderTypeKey);
        void InvalidateAll();
        void Invalidate(IShaderType* shaderTypeKey);

    private:
        std::unordered_map<IShaderType*, ShaderArchivePackage> shaderArchivePackages;
    };
}

namespace Render {
    class GlobalShader : public Shader {};

    template <typename Shader>
    class GlobalShaderType : public IShaderType {
    public:
        static GlobalShaderType& Get();

        GlobalShaderType();

        ~GlobalShaderType();
        NonCopyable(GlobalShaderType)

        std::string GetName() override;
        ShaderTypeKey GetHash() override;
        std::string GetCode() override;
        std::vector<VariantKey> GetVariants() override;
        std::vector<std::string> GetDefinitions(VariantKey variantKey) override;

    private:
        void ComputeVariantDefinitions();

        std::unordered_map<VariantKey, std::vector<std::string>> variantDefinitions;
    };

    template <typename T>
    class GlobalShaderMap {
    public:
        static GlobalShaderMap& Get(RHI::Device& device);

        ~GlobalShaderMap();
        NonCopyable(GlobalShaderMap)

        void Invalidate();
        ShaderInstance GetShaderInstance(const typename T::VariantSet& variantSet);

    private:
        explicit GlobalShaderMap(RHI::Device& inDevice);

        [[nodiscard]] const ShaderArchive& GetArchive(const typename T::VariantSet& variantSet) const;

        RHI::Device& device;
        std::unordered_map<VariantKey, Common::UniqueRef<RHI::ShaderModule>> shaderModules;
    };

    class GlobalShaderRegistry {
    public:
        static GlobalShaderRegistry& Get();

        GlobalShaderRegistry();
        ~GlobalShaderRegistry();
        NonCopyable(GlobalShaderRegistry)

        template <typename Shader>
        void Register();

        std::vector<IShaderType*>&& GetShaderTypes();

    private:
        std::vector<IShaderType*> shaderTypes;
    };

    class BoolShaderVariantFieldImpl {
    public:
        using ValueType = bool;

        static constexpr std::pair<uint32_t, uint32_t> valueRange = { 0, 1 };
        static constexpr ValueType defaultValue = false;

        BoolShaderVariantFieldImpl();
        BoolShaderVariantFieldImpl(BoolShaderVariantFieldImpl&& other) noexcept;
        ~BoolShaderVariantFieldImpl();

        void Set(ValueType inValue);
        [[nodiscard]] ValueType Get() const;
        [[nodiscard]] uint32_t GetNumberValue() const;

    private:
        uint32_t value;
    };

    template <uint32_t From, uint32_t To>
    struct RangedIntShaderVariantFieldImpl {
    public:
        using ValueType = uint32_t;

        static constexpr std::pair<uint32_t, uint32_t> valueRange = { From, To };
        static constexpr ValueType defaultValue = From;

        RangedIntShaderVariantFieldImpl();
        RangedIntShaderVariantFieldImpl(RangedIntShaderVariantFieldImpl&& other) noexcept;
        ~RangedIntShaderVariantFieldImpl();

        void Set(ValueType inValue);
        [[nodiscard]] ValueType Get() const;
        [[nodiscard]] uint32_t GetNumberValue() const;

    private:
        uint32_t value;
    };

    template <typename... Variants>
    class VariantSetImpl {
    public:
        static uint32_t VariantNum();

        VariantSetImpl();
        VariantSetImpl(const VariantSetImpl& other);
        VariantSetImpl(VariantSetImpl&& other) noexcept;
        ~VariantSetImpl();

        template <typename F>
        static void TraverseAll(F&& func);

        template <typename Variant>
        void Set(typename Variant::ValueType value);

        template <typename Variant>
        typename Variant::ValueType Get();

        [[nodiscard]] std::vector<std::string> ComputeDefinitions() const;
        [[nodiscard]] VariantKey Hash() const;

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

namespace Render {
    template <typename Shader>
    GlobalShaderType<Shader>& GlobalShaderType<Shader>::Get()
    {
        static GlobalShaderType instance;
        return instance;
    }

    template <typename Shader>
    GlobalShaderType<Shader>::GlobalShaderType()
    {
        ComputeVariantDefinitions();
    }

    template <typename Shader>
    GlobalShaderType<Shader>::~GlobalShaderType() = default;

    template <typename Shader>
    std::string GlobalShaderType<Shader>::GetName()
    {
        return Shader::name;
    }

    template <typename Shader>
    ShaderTypeKey GlobalShaderType<Shader>::GetHash()
    {
        return Common::HashUtils::CityHash(Shader::name, sizeof(Shader::name));
    }

    template <typename Shader>
    std::string GlobalShaderType<Shader>::GetCode()
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

    template <typename Shader>
    std::vector<VariantKey> GlobalShaderType<Shader>::GetVariants()
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

    template <typename Shader>
    std::vector<std::string> GlobalShaderType<Shader>::GetDefinitions(VariantKey variantKey)
    {
        auto iter = variantDefinitions.find(variantKey);
        Assert(iter != variantDefinitions.end());
        return iter->second;
    }

    template <typename Shader>
    void GlobalShaderType<Shader>::ComputeVariantDefinitions()
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

    template <typename T>
    GlobalShaderMap<T>& GlobalShaderMap<T>::Get(RHI::Device& device)
    {
        static std::unordered_map<RHI::Device*, Common::UniqueRef<GlobalShaderMap<T>>> map;
        auto iter = map.find(&device);
        if (iter == map.end()) {
            map[&device] = Common::UniqueRef<GlobalShaderMap<T>>(new GlobalShaderMap<T>(device));
        }
        return *map[&device];
    }

    template <typename T>
    GlobalShaderMap<T>::~GlobalShaderMap() = default;

    template <typename T>
    void GlobalShaderMap<T>::Invalidate()
    {
        auto variantNum = T::VariantSet::VariantNum();
        shaderModules.clear();
        shaderModules.reserve(variantNum);
    }

    template <typename T>
    ShaderInstance GlobalShaderMap<T>::GetShaderInstance(const typename T::VariantSet& variantSet)
    {
        auto variantKey = variantSet.Hash();
        const auto& archive = GetArchive(variantSet);

        auto iter = shaderModules.find(variantKey);
        if (iter != shaderModules.end()) {
            shaderModules[variantKey] = device.CreateShaderModule(RHI::ShaderModuleCreateInfo(T::entryPoint, archive.byteCode));
        }

        ShaderInstance result;
        result.rhiHandle = shaderModules[variantKey].Get();
        result.typeKey = GlobalShaderType<T>::Get().GetHash();
        result.variantKey = variantKey;
        result.reflectionData = &archive.reflectionData;
        return result;
    }

    template <typename T>
    GlobalShaderMap<T>::GlobalShaderMap(RHI::Device& inDevice)
        : device(inDevice)
    {
        auto variantNum = T::VariantSet::VariantNum();
        shaderModules.reserve(variantNum);
    }

    template<typename T>
    const ShaderArchive& GlobalShaderMap<T>::GetArchive(const typename T::VariantSet& variantSet) const
    {
        const auto& package = ShaderArchiveStorage::Get().GetShaderArchivePackage(&GlobalShaderType<T>::Get());

        auto iter = package.find(variantSet.Hash());
        Assert(iter != package.end());
        return iter->second;
    }

    template<typename Shader>
    void GlobalShaderRegistry::Register()
    {
        shaderTypes.emplace_back(&GlobalShaderType<Shader>::Get());
    }

    template<uint32_t From, uint32_t To>
    RangedIntShaderVariantFieldImpl<From, To>::RangedIntShaderVariantFieldImpl()
        : value(defaultValue)
    {
    }

    template<uint32_t From, uint32_t To>
    RangedIntShaderVariantFieldImpl<From, To>::RangedIntShaderVariantFieldImpl(RangedIntShaderVariantFieldImpl&& other) noexcept
        : value(other.value)
    {
    }

    template<uint32_t From, uint32_t To>
    RangedIntShaderVariantFieldImpl<From, To>::~RangedIntShaderVariantFieldImpl() = default;

    template<uint32_t From, uint32_t To>
    void RangedIntShaderVariantFieldImpl<From, To>::Set(RangedIntShaderVariantFieldImpl::ValueType inValue)
    {
        Assert(From <= value && value <= To);
        value = inValue;
    }

    template<uint32_t From, uint32_t To>
    typename RangedIntShaderVariantFieldImpl<From, To>::ValueType RangedIntShaderVariantFieldImpl<From, To>::Get() const
    {
        return value;
    }

    template<uint32_t From, uint32_t To>
    uint32_t RangedIntShaderVariantFieldImpl<From, To>::GetNumberValue() const
    {
        return value;
    }

    template<typename... Variants>
    uint32_t VariantSetImpl<Variants...>::VariantNum()
    {
        uint32_t result = 1;
        (void) std::initializer_list<int> { ([&result]() -> void {
            auto valueRange = Variants::valueRange;
            Assert(valueRange.first <= valueRange.second);
            result *= valueRange.second - valueRange.first + 1;
        }(), 0)... };
        return result;
    }

    template<typename... Variants>
    VariantSetImpl<Variants...>::VariantSetImpl()
    {
        (void) std::initializer_list<int> { ([this]() -> void { std::get<Variants>(variants).Set(Variants::defaultValue); }(), 0)... };
    }

    template<typename... Variants>
    VariantSetImpl<Variants...>::VariantSetImpl(const VariantSetImpl& other)
    {
        (void) std::initializer_list<int> { ([this, &other]() -> void { std::get<Variants>(variants).Set(std::get<Variants>(other.variants).Get()); }(), 0)... };
    }

    template<typename... Variants>
    VariantSetImpl<Variants...>::VariantSetImpl(VariantSetImpl&& other) noexcept
        : variants(std::move(other.variants))
    {
    }

    template<typename... Variants>
    VariantSetImpl<Variants...>::~VariantSetImpl() = default;

    template<typename... Variants>
    template<typename F>
    void VariantSetImpl<Variants...>::TraverseAll(F&& func)
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

    template<typename... Variants>
    template<typename Variant>
    void VariantSetImpl<Variants...>::Set(typename Variant::ValueType value)
    {
        std::get<Variant>(variants).Set(value);
    }

    template<typename... Variants>
    template<typename Variant>
    typename Variant::ValueType VariantSetImpl<Variants...>::Get()
    {
        return std::get<Variant>(variants).Get();
    }

    template<typename... Variants>
    std::vector<std::string> VariantSetImpl<Variants...>::ComputeDefinitions() const
    {
        std::vector<std::string> result;
        result.reserve(sizeof...(Variants));
        (void) std::initializer_list<int> { ([&result, this]() -> void {
            result.emplace_back(std::string(Variants::macro) + "=" + std::to_string(std::get<Variants>(variants).GetNumberValue()));
        }(), 0)... };
        return result;
    }

    template<typename... Variants>
    VariantKey VariantSetImpl<Variants...>::Hash() const
    {
        return Common::HashUtils::CityHash(&variants, sizeof(std::tuple<Variants...>));
    }
}
