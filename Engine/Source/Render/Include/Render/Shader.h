//
// Created by johnk on 2022/7/24.
//

#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include <tuple>
#include <array>

#include <RHI/RHI.h>

#define StaticShaderInfo(inClass, inName, inStage, inSourceFile, inEntryPoint) \
    public: \
        \
        static const inClass& Get(); \
        \
    private: \
        friend class StaticShaderType<inClass>; \
        \
        static constexpr const char* name = inName; \
        static constexpr RHI::ShaderStageBits stage = inStage; \
        static constexpr const char* sourceFile = inSourceFile; \
        static constexpr const char* entryPoint = inEntryPoint; \

#define BoolVariantField(inName, inMacro, inDefaultValue) \
    struct inName { \
        static constexpr Render::VariantFieldType type = Render::VariantFieldType::vfBool; \
        static constexpr const char* macro = #inMacro; \
        static constexpr bool defaultValue = inDefaultValue; \
    }; \

#define RangedIntVariantField(inName, inMacro, inDefaultValue, inFrom, inTo) \
    struct inName { \
        static constexpr Render::VariantFieldType type = Render::VariantFieldType::vfRangedInt; \
        static constexpr const char* macro = #inMacro; \
        static constexpr int32_t defaultValue = inDefaultValue; \
        static constexpr int32_t from = inFrom; \
        static constexpr int32_t to = inTo; \
    }; \

#define MakeVariantFieldVec(...) \
    using VariantFieldVec = std::tuple<__VA_ARGS__>; \

#define EmptyVariantFieldVec \
    using VariantFieldVec = std::tuple<>; \

#define BeginIncludeDirectories \
    static constexpr auto includeDirectories = std::to_array<std::string_view>({ \

#define EndIncludeDirectories \
    }); \

#define EmptyIncludeDirectories \
    static constexpr auto includeDirectories = std::array<std::string_view, 0> {}; \

#define ImplementStaticShaderType(inClass) \
    static inClass __staticShaderTypeInstance_##inClass = inClass(); \
    \
    const inClass& inClass::Get() { \
        return __staticShaderTypeInstance_##inClass; \
    } \

namespace Render {
    enum class VariantFieldType : uint8_t {
        vfBool,
        vfRangedInt,
        max
    };

    struct ShaderBoolVariantField {
        std::string macro;
        bool defaultValue;

        bool operator==(const ShaderBoolVariantField& inRhs) const;
    };

    struct ShaderRangedIntVariantField {
        std::string macro;
        int32_t defaultValue;
        std::pair<int32_t, int32_t> range;

        bool operator==(const ShaderRangedIntVariantField& inRhs) const;
    };

    using ShaderTypeKey = uint64_t;
    using ShaderVariantKey = uint64_t;
    using ShaderSourceHash = uint64_t;
    using ShaderVariantField = std::variant<ShaderBoolVariantField, ShaderRangedIntVariantField>;
    using ShaderVariantFieldVec = std::vector<ShaderVariantField>;
    using ShaderVariantValue = std::variant<bool, int32_t>;
    using ShaderVariantValueMap = std::unordered_map<std::string, ShaderVariantValue>;

    constexpr ShaderSourceHash shaderSourceHashNotCompiled = 0;

    class ShaderUtils {
    public:
        static std::vector<ShaderVariantValueMap> GetAllVariants(const ShaderVariantFieldVec& inFields);
        static ShaderVariantKey ComputeVariantKey(const ShaderVariantFieldVec& inFields, const ShaderVariantValueMap& inVariantSet);
        static std::vector<std::string> ComputeVariantDefinitions(const ShaderVariantFieldVec& inFields, const ShaderVariantValueMap& inVariantSet);
        static ShaderSourceHash ComputeShaderSourceHash(const std::string& inSourceFile, const std::vector<std::string>& inIncludeDirectories);

    private:
        static std::string GetAbsoluteIncludeFile(const std::string& inPath, const std::vector<std::string>& inIncludeDirectories);
        static void GatherShaderSources(std::unordered_map<std::string, std::string>& outFileAndSource, const std::string& inSourceFile, const std::vector<std::string>& inIncludeDirectories);
    };

    struct VertexFactoryInput {
        std::string name;
        RHI::VertexFormat vertexFormat;
        uint32_t offset;
    };

    using VertexFactoryInputVec = std::vector<VertexFactoryInput>;

    class VertexFactoryType {
    public:
        virtual ~VertexFactoryType();

        virtual const std::string& GetName() const = 0;
        virtual const std::string& GetSourceFile() const = 0;
        virtual const VertexFactoryInputVec& GetVertexInputs() const = 0;
        virtual const ShaderVariantFieldVec& GetVariantFields() const = 0;

    protected:
        VertexFactoryType();
    };

    template <typename T>
    class StaticVertexFactoryType : public VertexFactoryType {
    public:
        StaticVertexFactoryType();
        ~StaticVertexFactoryType() override;

        const std::string& GetName() const override;
        const std::string& GetSourceFile() const override;
        const VertexFactoryInputVec& GetVertexInputs() const override;
        const ShaderVariantFieldVec& GetVariantFields() const override;
    };

    class ShaderType {
    public:
        NonCopyable(ShaderType)
        NonMovable(ShaderType)

        virtual ~ShaderType();
        virtual const std::string& GetName() const = 0;
        virtual RHI::ShaderStageBits GetStage() const = 0;
        virtual const std::string& GetSourceFile() const = 0;
        virtual const std::string& GetEntryPoint() const = 0;
        virtual const std::vector<std::string>& GetIncludeDirectories() const = 0;
        virtual const ShaderVariantFieldVec& GetVariantFields() const = 0;

        ShaderTypeKey GetKey() const;

    protected:
        static ShaderTypeKey MakeTypeKeyFromName(const std::string& inName);

        explicit ShaderType(ShaderTypeKey inKey);

        ShaderTypeKey key;
    };

    template <typename T>
    class StaticShaderType : public ShaderType {
    public:
        StaticShaderType();
        ~StaticShaderType() override;

        const std::string& GetName() const override;
        RHI::ShaderStageBits GetStage() const override;
        const std::string& GetSourceFile() const override;
        const std::string& GetEntryPoint() const override;
        const std::vector<std::string>& GetIncludeDirectories() const override;
        const ShaderVariantFieldVec& GetVariantFields() const override;

    private:
        template <size_t... I> void InitVariantFieldsFromStatic(std::index_sequence<I...>);

        std::string name;
        std::string sourceFile;
        std::string entryPoint;
        std::vector<std::string> includeDirectories;
        ShaderVariantFieldVec variantFields;
    };

    class MaterialShaderType final : public ShaderType {
    public:
        MaterialShaderType(
            const VertexFactoryType& inVertexFactory,
            std::string inName,
            RHI::ShaderStageBits inStage,
            std::string inSourceFile,
            std::string inEntryPoint,
            const std::vector<std::string>& inIncludeDirectories,
            const ShaderVariantFieldVec& inShaderVariantFields);
        ~MaterialShaderType() override;

        const std::string& GetName() const override;
        RHI::ShaderStageBits GetStage() const override;
        const std::string& GetSourceFile() const override;
        const std::string& GetEntryPoint() const override;
        const std::vector<std::string>& GetIncludeDirectories() const override;
        const ShaderVariantFieldVec& GetVariantFields() const override;

    private:
        const VertexFactoryType& vertexFactory;
        std::string name;
        RHI::ShaderStageBits stage;
        std::string sourceFile;
        std::string entryPoint;
        std::vector<std::string> includeDirectories;
        ShaderVariantFieldVec shaderVariantFields;
    };

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

    struct ShaderModuleData {
        std::string entryPoint;
        std::vector<uint8_t> byteCode;
        ShaderReflectionData reflectionData;
    };

    struct ShaderInstance {
        ShaderTypeKey typeKey;
        ShaderVariantKey variantKey;
        RHI::ShaderModule* rhiHandle;
        const ShaderReflectionData* reflectionData;

        ShaderInstance();

        bool Valid() const;
        uint64_t Hash() const;
    };

    class ShaderRegistry {
    public:
        static ShaderRegistry& Get();

        ~ShaderRegistry();

        void RegisterType(const ShaderType& inShaderType);
        void UnregisterType(const ShaderType& inShaderType);
        void ResetType(const ShaderType& inShaderType);
        void ResetAllTypes();
        const ShaderType& GetType(ShaderTypeKey inKey) const;
        std::vector<const ShaderType*> AllTypes() const;
        ShaderInstance GetShaderInstance(RHI::Device& inDevice, const ShaderType& inShaderType, const ShaderVariantValueMap& inShaderVariants);

    private:
        friend class ShaderTypeCompiler;

        using DeviceShaderModulesMap = std::unordered_map<ShaderVariantKey, Common::UniquePtr<RHI::ShaderModule>>;

        struct ShaderStorage {
            const ShaderType* shaderType;
            ShaderSourceHash sourceHash;
            std::unordered_map<ShaderVariantKey, ShaderModuleData> shaderModuleDatas;
            std::unordered_map<RHI::Device*, DeviceShaderModulesMap> deviceShaderModules;
        };

        ShaderRegistry();

        std::unordered_map<ShaderTypeKey, ShaderStorage> shaderStorages;
    };
}

namespace Render {
    template <typename T>
    StaticVertexFactoryType<T>::StaticVertexFactoryType() = default;

    template <typename T>
    StaticVertexFactoryType<T>::~StaticVertexFactoryType() = default;

    template <typename T>
    const std::string& StaticVertexFactoryType<T>::GetName() const
    {
        return T::name;
    }

    template <typename T>
    const std::string& StaticVertexFactoryType<T>::GetSourceFile() const
    {
        return T::sourceFile;
    }

    template <typename T>
    const VertexFactoryInputVec& StaticVertexFactoryType<T>::GetVertexInputs() const
    {
        return T::vertexInputs;
    }

    template <typename T>
    const ShaderVariantFieldVec& StaticVertexFactoryType<T>::GetVariantFields() const
    {
        return T::variantFields;
    }

    template <typename T>
    StaticShaderType<T>::StaticShaderType()
        : ShaderType(MakeTypeKeyFromName(T::name))
        , name(T::name)
        , sourceFile(T::sourceFile)
        , entryPoint(T::entryPoint)
    {
        includeDirectories.reserve(T::includeDirectories.size());
        for (const auto& includeDir : T::includeDirectories) {
            includeDirectories.emplace_back(includeDir.data(), includeDir.size());
        }

        InitVariantFieldsFromStatic(std::make_index_sequence<std::tuple_size_v<typename T::VariantFieldVec>> {});
    }

    template <typename T>
    StaticShaderType<T>::~StaticShaderType() = default;

    template <typename T>
    const std::string& StaticShaderType<T>::GetName() const
    {
        return name;
    }

    template <typename T>
    RHI::ShaderStageBits StaticShaderType<T>::GetStage() const
    {
        return T::stage;
    }

    template <typename T>
    const std::string& StaticShaderType<T>::GetSourceFile() const
    {
        return sourceFile;
    }

    template <typename T>
    const std::string& StaticShaderType<T>::GetEntryPoint() const
    {
        return entryPoint;
    }

    template <typename T>
    const std::vector<std::string>& StaticShaderType<T>::GetIncludeDirectories() const
    {
        return includeDirectories;
    }

    template <typename T>
    const ShaderVariantFieldVec& StaticShaderType<T>::GetVariantFields() const
    {
        return variantFields;
    }

    template <typename T>
    template <size_t... I>
    void StaticShaderType<T>::InitVariantFieldsFromStatic(std::index_sequence<I...>)
    {
        (void) std::initializer_list<int> { ([&]() -> void {
            using StaticVariantField = std::tuple_element_t<I, typename T::VariantFieldVec>;
            constexpr VariantFieldType type = StaticVariantField::type; // NOLINT

            if constexpr (type == VariantFieldType::vfBool) {
                variantFields.emplace_back(ShaderBoolVariantField { StaticVariantField::macro, StaticVariantField::defaultValue });
            } else if constexpr (type == VariantFieldType::vfRangedInt) {
                variantFields.emplace_back(ShaderRangedIntVariantField { StaticVariantField::macro, StaticVariantField::defaultValue, { StaticVariantField::from, StaticVariantField::to } });
            } else {
                static_assert(false);
            }
        }(), 0)... };
    }
} // namespace Render
