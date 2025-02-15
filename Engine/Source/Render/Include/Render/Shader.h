//
// Created by johnk on 2022/7/24.
//

#pragma once

#include <vector>
#include <unordered_map>
#include <utility>
#include <tuple>

#include <Common/Utility.h>
#include <Common/Debug.h>
#include <Common/Hash.h>
#include <Common/File.h>
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

    class IShaderType {
    public:
        virtual ~IShaderType();
        virtual const std::string& GetName() = 0;
        virtual ShaderTypeKey GetKey() = 0;
        virtual RHI::ShaderStageBits GetStage() = 0;
        virtual const std::string& GetEntryPoint() = 0;
        virtual const std::string& GetCode() = 0;
        virtual uint32_t GetVariantNum() = 0;
        virtual const std::vector<VariantKey>& GetVariants() = 0;
        virtual const std::vector<std::string>& GetDefinitions(VariantKey variantKey) = 0;
        virtual void Reload() = 0;
        virtual void Invalidate() = 0;
    };

    struct ShaderInstance {
        RHI::ShaderModule* rhiHandle = nullptr;
        ShaderTypeKey typeKey = 0;
        VariantKey variantKey = 0;
        const ShaderReflectionData* reflectionData = nullptr;

        bool IsValid() const;
        size_t Hash() const;
    };

    using ShaderArchivePackage = std::unordered_map<VariantKey, ShaderArchive>;

    class ShaderArchiveStorage {
    public:
        static ShaderArchiveStorage& Get();
        ShaderArchiveStorage();
        ~ShaderArchiveStorage();
        NonCopyable(ShaderArchiveStorage)

        void UpdateShaderArchivePackage(ShaderTypeKey shaderTypeKey, ShaderArchivePackage&& shaderArchivePackage);
        const ShaderArchivePackage& GetShaderArchivePackage(ShaderTypeKey shaderTypeKey);
        void InvalidateAll();
        void Invalidate(ShaderTypeKey shaderTypeKey);

    private:
        std::unordered_map<ShaderTypeKey, ShaderArchivePackage> shaderArchivePackages;
    };
}

namespace Render {
    class GlobalShader : public Shader {};

    template <typename Shader>
    class GlobalShaderType final : public IShaderType {
    public:
        static GlobalShaderType& Get();

        GlobalShaderType();

        ~GlobalShaderType() override;
        NonCopyable(GlobalShaderType)

        const std::string& GetName() override;
        ShaderTypeKey GetKey() override;
        RHI::ShaderStageBits GetStage() override;
        const std::string& GetEntryPoint() override;
        const std::string& GetCode() override;
        uint32_t GetVariantNum() override;
        const std::vector<VariantKey>& GetVariants() override;
        const std::vector<std::string>& GetDefinitions(VariantKey variantKey) override;
        void Reload() override;
        void Invalidate() override;

    private:
        void ReloadInternal();
        void ReadCode();
        void ComputeVariants();
        void ComputeVariantDefinitions();

        std::string name;
        ShaderTypeKey key;
        RHI::ShaderStageBits stage;
        std::string entryPoint;
        std::string code;
        std::vector<VariantKey> variants;
        std::unordered_map<VariantKey, std::vector<std::string>> variantDefinitions;
    };

    template <typename T>
    class GlobalShaderMap {
    public:
        static GlobalShaderMap& Get();

        ~GlobalShaderMap();
        NonCopyable(GlobalShaderMap)

        void Invalidate();
        ShaderInstance GetShaderInstance(RHI::Device& device, const typename T::VariantSet& variantSet);

    private:
        GlobalShaderMap();

        [[nodiscard]] const ShaderArchive& GetArchive(const typename T::VariantSet& variantSet) const;

        std::unordered_map<RHI::Device*, std::unordered_map<VariantKey, Common::UniquePtr<RHI::ShaderModule>>> shaderModules;
    };

    class GlobalShaderRegistry {
    public:
        static GlobalShaderRegistry& Get();

        GlobalShaderRegistry();
        ~GlobalShaderRegistry();
        NonCopyable(GlobalShaderRegistry)

        template <typename Shader>
        void Register();

        const std::vector<IShaderType*>& GetShaderTypes() const;
        // call this func before device release
        void InvalidateAll() const;
        // call this func after shader reloaded
        void ReloadAll() const;

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
        ~RangedIntShaderVariantFieldImpl(); // NOLINT

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
    static constexpr RHI::ShaderStageBits stage = inStage; \

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

#define NonVariant \
    RangedIntShaderVariantField(_PlaceholderVariantField, "__PLACEHOLDER_VARIANT", 0, 0); /* NOLINT */ \
    VariantSet(_PlaceholderVariantField);

#define RegisterGlobalShader(inClass) \
    static uint8_t _globalShaderRegister_##inClass = []() -> uint8_t { \
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
        ReloadInternal();
    }

    template <typename Shader>
    GlobalShaderType<Shader>::~GlobalShaderType() = default;

    template <typename Shader>
    const std::string& GlobalShaderType<Shader>::GetName()
    {
        return name;
    }

    template <typename Shader>
    ShaderTypeKey GlobalShaderType<Shader>::GetKey()
    {
        return key;
    }

    template <typename Shader>
    RHI::ShaderStageBits GlobalShaderType<Shader>::GetStage()
    {
        return stage;
    }

    template <typename Shader>
    const std::string& GlobalShaderType<Shader>::GetEntryPoint()
    {
        return entryPoint;
    }

    template <typename Shader>
    const std::string& GlobalShaderType<Shader>::GetCode()
    {
        return code;
    }

    template <typename Shader>
    uint32_t GlobalShaderType<Shader>::GetVariantNum()
    {
        return variants.size();
    }

    template <typename Shader>
    const std::vector<VariantKey> & GlobalShaderType<Shader>::GetVariants()
    {
        return variants;
    }

    template <typename Shader>
    const std::vector<std::string>& GlobalShaderType<Shader>::GetDefinitions(VariantKey variantKey)
    {
        return variantDefinitions.at(variantKey);
    }

    template <typename Shader>
    void GlobalShaderType<Shader>::Reload()
    {
        Invalidate();
        ReloadInternal();
    }

    template <typename Shader>
    void GlobalShaderType<Shader>::Invalidate()
    {
        GlobalShaderMap<Shader>::Get().Invalidate();
    }

    template <typename Shader>
    void GlobalShaderType<Shader>::ReloadInternal()
    {
        name = Shader::name;
        const std::string keySource = std::string("Global-") + name;
        key = Common::HashUtils::CityHash(keySource.data(), keySource.size());
        stage = Shader::stage;
        entryPoint = Shader::entryPoint;

        ReadCode();
        ComputeVariants();
        ComputeVariantDefinitions();
    }

    template <typename Shader>
    void GlobalShaderType<Shader>::ReadCode()
    {
        static std::unordered_map<std::string, std::string> pathMap = {
            { "/Engine/Shader", Core::Paths::EngineShaderDir().String() }
        };

        const std::string sourceFile = Shader::sourceFile;
        for (const auto& [mapFrom, mapTo] : pathMap) {
            if (sourceFile.starts_with(mapFrom)) {
                code = Common::FileUtils::ReadTextFile(Common::StringUtils::Replace(sourceFile, mapFrom, mapTo));
                return;
            }
        }
        code = Common::FileUtils::ReadTextFile(sourceFile);
    }

    template <typename Shader>
    void GlobalShaderType<Shader>::ComputeVariants()
    {
        variants.reserve(Shader::VariantSet::VariantNum());
        Shader::VariantSet::TraverseAll([this](auto&& variantSetImpl) -> void {
            const auto* variantSet = static_cast<typename Shader::VariantSet*>(&variantSetImpl);
            if (!Shader::VariantFilter(*variantSet)) {
                return;
            }
            variants.emplace_back(variantSet->Hash());
        });
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
    GlobalShaderMap<T>& GlobalShaderMap<T>::Get()
    {
        static GlobalShaderMap instance;
        return instance;
    }

    template <typename T>
    GlobalShaderMap<T>::~GlobalShaderMap() = default;

    template <typename T>
    void GlobalShaderMap<T>::Invalidate()
    {
        shaderModules.clear();
    }

    template <typename T>
    ShaderInstance GlobalShaderMap<T>::GetShaderInstance(RHI::Device& device, const typename T::VariantSet& variantSet)
    {
        auto variantKey = variantSet.Hash();
        const auto& archive = GetArchive(variantSet);
        auto& deviceShaderModules = shaderModules[&device];

        auto iter = deviceShaderModules.find(variantKey);
        if (iter == deviceShaderModules.end()) {
            deviceShaderModules[variantKey] = device.CreateShaderModule(RHI::ShaderModuleCreateInfo(T::entryPoint, archive.byteCode));
        }

        ShaderInstance result;
        result.rhiHandle = deviceShaderModules.at(variantKey).Get();
        result.typeKey = GlobalShaderType<T>::Get().GetKey();
        result.variantKey = variantKey;
        result.reflectionData = &archive.reflectionData;
        return result;
    }

    template <typename T>
    GlobalShaderMap<T>::GlobalShaderMap() = default;

    template<typename T>
    const ShaderArchive& GlobalShaderMap<T>::GetArchive(const typename T::VariantSet& variantSet) const
    {
        const auto& package = ShaderArchiveStorage::Get().GetShaderArchivePackage(GlobalShaderType<T>::Get().GetKey());

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
    void RangedIntShaderVariantFieldImpl<From, To>::Set(const ValueType inValue)
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
        std::vector<VariantSetImpl> variantSets;
        variantSets.reserve(VariantNum());
        variantSets.emplace_back(VariantSetImpl());

        (void) std::initializer_list<int> { ([&variantSets]() -> void {
            auto valueRange = Variants::valueRange;
            auto variantSetsSize = variantSets.size();
            for (auto i = valueRange.first; i <= valueRange.second; ++i) {
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
