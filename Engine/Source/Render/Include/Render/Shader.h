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

#define ShaderTypeInfo(inClass, inStage, inSourceFile, inEntryPoint) \
    public: \
        \
        static const inClass& Get(); \
        \
        static constexpr const char* name = #inClass; \
        static constexpr RHI::ShaderStageBits stage = inStage; \
        static constexpr const char* sourceFile = inSourceFile; \
        static constexpr const char* entryPoint = inEntryPoint; \

#define DeclBoolVariantField(inName, inMacro, inDefaultValue) \
    struct inName { \
        static constexpr Render::VariantFieldType type = Render::VariantFieldType::vfBool; \
        static constexpr const char* macro = #inMacro; \
        static constexpr bool defaultValue = inDefaultValue; \
    }; \

#define DeclRangedIntVariantField(inName, inMacro, inDefaultValue, inFrom, inTo) \
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

#define VertexFactoryTypeInfo(inClass, inSourceFile) \
    public: \
        \
        static const inClass& Get(); \
        \
        static constexpr const char* name = #inClass; \
        static constexpr const char* sourceFile = inSourceFile; \

#define DeclVertexInput(inType, inName, inFormat, inOffset) \
    struct inType { \
        static constexpr const char* name = #inName; \
        static constexpr RHI::VertexFormat format = inFormat; \
        static constexpr uint32_t offset = inOffset; \
    }; \

#define MakeVertexInputVec(...) \
    using VertexFactoryInputVec = std::tuple<__VA_ARGS__>; \

#define BeginSupportedMaterialTypes \
    static constexpr auto supportedMaterialTypes = std::to_array<Render::MaterialType>({ \

#define EndSupportedMaterialTypes \
    }); \

#define ImplementStaticVertexFactoryType(inClass) \
    static inClass __staticVertexFactoryTypeInstance_##inClass = inClass(); \
    \
    const inClass& inClass::Get() { \
        return __staticVertexFactoryTypeInstance_##inClass; \
    } \

namespace Render::Internal {
    static uint64_t MakeTypeKeyFromName(const std::string& inName);
}

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
        RHI::VertexFormat format;
        uint32_t offset;

        bool operator==(const VertexFactoryInput& inRhs) const;
    };

    using VertexFactoryInputVec = std::vector<VertexFactoryInput>;
    using VertexFactoryTypeKey = uint64_t;

    enum class MaterialType : uint8_t {
        surface,
        volume,
        postProcess,
        max
    };

    class VertexFactoryType {
    public:
        virtual ~VertexFactoryType();

        virtual const std::string& GetName() const = 0;
        virtual const std::string& GetSourceFile() const = 0;
        virtual const VertexFactoryInputVec& GetVertexInputs() const = 0;
        virtual const ShaderVariantFieldVec& GetVariantFields() const = 0;
        virtual bool SupportMaterialType(MaterialType inType) const = 0;

        VertexFactoryTypeKey GetKey() const;

    protected:
        explicit VertexFactoryType(VertexFactoryTypeKey inKey);

    private:
        VertexFactoryTypeKey key;
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
        bool SupportMaterialType(MaterialType inType) const override;

    private:
        std::string name;
        std::string sourceFile;
        VertexFactoryInputVec inputs;
        ShaderVariantFieldVec variantFields;
        std::unordered_set<MaterialType> supportedMaterialTypes;
    };

    class VertexFactoryRegistry {
    public:
        static VertexFactoryRegistry& Get();

        ~VertexFactoryRegistry();

        void RegisterType(const VertexFactoryType* inType);
        void UnregisterType(const VertexFactoryType* inType);
        std::vector<const VertexFactoryType*> AllTypes() const;

    private:
        VertexFactoryRegistry();

        std::unordered_map<VertexFactoryTypeKey, const VertexFactoryType*> types;
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
        explicit ShaderType(ShaderTypeKey inKey);

    private:
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

        // TODO consider thread issue !!! maybe shaderType, sourceHash and shaderModuleDatas need updated by game thread and copy to render thread
        // TODO , and deviceShaderModules only be accessed in render thread, best to perform a thread data copy
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

namespace Render::Internal {
    static uint64_t MakeTypeKeyFromName(const std::string& inName)
    {
        return Common::HashUtils::CityHash(inName.data(), inName.size());
    }

    template <typename T, size_t... I>
    static ShaderVariantFieldVec BuildVariantFieldVecFromStatic(std::index_sequence<I...>)
    {
        ShaderVariantFieldVec result;
        result.reserve(sizeof...(I));
        (void) std::initializer_list<int> { ([&]() -> void {
            using StaticVariantField = std::tuple_element_t<I, typename T::VariantFieldVec>;
            constexpr VariantFieldType type = StaticVariantField::type; // NOLINT

            if constexpr (type == VariantFieldType::vfBool) {
                result.emplace_back(ShaderBoolVariantField { StaticVariantField::macro, StaticVariantField::defaultValue });
            } else if constexpr (type == VariantFieldType::vfRangedInt) {
                result.emplace_back(ShaderRangedIntVariantField { StaticVariantField::macro, StaticVariantField::defaultValue, { StaticVariantField::from, StaticVariantField::to } });
            } else {
                QuickFail();
            }
        }(), 0)... };
        return result;
    }

    template <typename T, size_t... I>
    static VertexFactoryInputVec BuildVertexFactoryInputVecFromStatic(std::index_sequence<I...>)
    {
        VertexFactoryInputVec result;
        result.reserve(sizeof...(I));
        (void) std::initializer_list<int> { ([&]() -> void {
            using VertexFactoryInputStruct = std::tuple_element_t<I, typename T::VertexFactoryInputVec>;
            result.emplace_back(VertexFactoryInput { VertexFactoryInputStruct::name, VertexFactoryInputStruct::format, VertexFactoryInputStruct::offset });
        }(), 0)... };
        return result;
    }
}

namespace Render {
    template <typename T>
    StaticVertexFactoryType<T>::StaticVertexFactoryType()
        : VertexFactoryType(Internal::MakeTypeKeyFromName(T::name))
        , name(T::name)
        , sourceFile(T::sourceFile)
    {
        inputs = Internal::BuildVertexFactoryInputVecFromStatic<T>(std::make_index_sequence<std::tuple_size_v<typename T::VertexFactoryInputVec>> {});
        variantFields = Internal::BuildVariantFieldVecFromStatic<T>(std::make_index_sequence<std::tuple_size_v<typename T::VariantFieldVec>> {});

        for (const auto& materialType : T::supportedMaterialTypes) {
            supportedMaterialTypes.emplace(materialType);
        }
    }

    template <typename T>
    StaticVertexFactoryType<T>::~StaticVertexFactoryType() = default;

    template <typename T>
    const std::string& StaticVertexFactoryType<T>::GetName() const
    {
        return name;
    }

    template <typename T>
    const std::string& StaticVertexFactoryType<T>::GetSourceFile() const
    {
        return sourceFile;
    }

    template <typename T>
    const VertexFactoryInputVec& StaticVertexFactoryType<T>::GetVertexInputs() const
    {
        return inputs;
    }

    template <typename T>
    const ShaderVariantFieldVec& StaticVertexFactoryType<T>::GetVariantFields() const
    {
        return variantFields;
    }

    template <typename T>
    bool StaticVertexFactoryType<T>::SupportMaterialType(MaterialType inType) const
    {
        return supportedMaterialTypes.contains(inType);
    }

    template <typename T>
    StaticShaderType<T>::StaticShaderType()
        : ShaderType(Internal::MakeTypeKeyFromName(T::name))
        , name(T::name)
        , sourceFile(T::sourceFile)
        , entryPoint(T::entryPoint)
    {
        includeDirectories.reserve(T::includeDirectories.size());
        for (const auto& includeDir : T::includeDirectories) {
            includeDirectories.emplace_back(includeDir.data(), includeDir.size());
        }

        variantFields = Internal::BuildVariantFieldVecFromStatic<T>(std::make_index_sequence<std::tuple_size_v<typename T::VariantFieldVec>> {});
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
} // namespace Render
