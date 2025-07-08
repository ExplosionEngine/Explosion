//
// Created by johnk on 2025/3/21.
//

#pragma once

#include <unordered_map>

#include <Common/Math/Vector.h>
#include <Common/Math/Matrix.h>
#include <Runtime/Asset/Asset.h>
#include <Runtime/Asset/Texture.h>
#include <Runtime/Meta.h>
#include <Runtime/Api.h>

namespace Runtime {
    enum class EEnum() MaterialType : uint8_t {
        surface,
        volume,
        postProcess,
        max
    };
    static_assert(static_cast<uint8_t>(MaterialType::max) == static_cast<uint8_t>(Render::MaterialType::max));

    struct RUNTIME_API EClass() MaterialBoolVariantField {
        EClassBody(MaterialBoolVariantField)

        MaterialBoolVariantField();

        EProperty() bool defaultValue;
        EProperty() uint8_t sortPriority;
    };

    struct RUNTIME_API EClass() MaterialRangedUintVariantField {
        EClassBody(MaterialRangedUintVariantField)

        MaterialRangedUintVariantField();

        EProperty() uint8_t defaultValue;
        EProperty() std::pair<uint8_t, uint8_t> range;
        EProperty() uint8_t sortPriority;
    };

    struct RUNTIME_API EClass() MaterialBoolParameterField {
        EClassBody(MaterialBoolParameterField)

        MaterialBoolParameterField();

        EProperty() bool defaultValue;
        EProperty() uint8_t sortPriority;
    };

    struct RUNTIME_API EClass() MaterialIntParameterField {
        EClassBody(MaterialIntParameterField)

        MaterialIntParameterField();

        EProperty() int32_t defaultValue;
        EProperty() std::optional<std::pair<int32_t, int32_t>> range;
        EProperty() uint8_t sortPriority;
    };

    struct RUNTIME_API EClass() MaterialFloatParameterField {
        EClassBody(MaterialFloatParameterField)

        MaterialFloatParameterField();

        EProperty() float defaultValue;
        EProperty() std::optional<std::pair<float, float>> range;
        EProperty() uint8_t sortPriority;
    };

    struct RUNTIME_API EClass() MaterialFVec2ParameterField {
        EClassBody(MaterialFVec2ParameterField)

        MaterialFVec2ParameterField();

        EProperty() Common::FVec2 defaultValue;
        EProperty() std::optional<std::pair<Common::FVec2, Common::FVec2>> range;
        EProperty() uint8_t sortPriority;
    };

    struct RUNTIME_API EClass() MaterialFVec3ParameterField {
        EClassBody(MaterialFVec3ParameterField)

        MaterialFVec3ParameterField();

        EProperty() Common::FVec3 defaultValue;
        EProperty() std::optional<std::pair<Common::FVec3, Common::FVec3>> range;
        EProperty() uint8_t sortPriority;
    };

    struct RUNTIME_API EClass() MaterialFVec4ParameterField {
        EClassBody(MaterialFVec4ParameterField)

        MaterialFVec4ParameterField();

        EProperty() Common::FVec4 defaultValue;
        EProperty() std::optional<std::pair<Common::FVec4, Common::FVec4>> range;
        EProperty() uint8_t sortPriority;
    };

    struct RUNTIME_API EClass() MaterialFMat4x4ParameterField {
        EClassBody(MaterialFMat4x4ParameterField)

        MaterialFMat4x4ParameterField();

        EProperty() Common::FMat4x4 defaultValue;
        EProperty() uint8_t sortPriority;
    };

    struct RUNTIME_API EClass() MaterialTextureParameterField {
        EClassBody(MaterialTextureParameterField)

        MaterialTextureParameterField();

        EProperty() AssetPtr<Texture> defaultValue;
        EProperty() uint8_t sortPriority;
    };

    struct RUNTIME_API EClass() MaterialRenderTargetParameterField {
        EClassBody(MaterialRenderTargetParameterField)

        MaterialRenderTargetParameterField();

        EProperty() AssetPtr<RenderTarget> defaultValue;
        EProperty() uint8_t sortPriority;
    };

    class RUNTIME_API EClass() Material final : public Asset {
        EPolyClassBody(Material)

    public:
        using VariantField = std::variant<
            MaterialBoolVariantField,
            MaterialRangedUintVariantField
        >;
        using VariantFieldMap = std::unordered_map<std::string, VariantField>;

        using ParameterField = std::variant<
            MaterialBoolParameterField,
            MaterialIntParameterField,
            MaterialFloatParameterField,
            MaterialFVec2ParameterField,
            MaterialFVec3ParameterField,
            MaterialFVec4ParameterField,
            MaterialFMat4x4ParameterField,
            MaterialTextureParameterField,
            MaterialRenderTargetParameterField
        >;
        using ParameterFieldMap = std::unordered_map<std::string, ParameterField>;

        explicit Material(Core::Uri inUri);

        NonCopyable(Material)
        NonMovable(Material)

        EFunc() MaterialType GetType() const;
        EFunc() void SetType(MaterialType inType);
        EFunc() const std::string& GetSource() const;
        EFunc() void SetSource(const std::string& inSource);

        EFunc() bool HasVariantField(const std::string& inName) const;
        EFunc() VariantField& GetVariantField(const std::string& inName);
        EFunc() const VariantField& GetVariantField(const std::string& inName) const;
        EFunc() VariantField* FindVariantField(const std::string& inName);
        EFunc() const VariantField* FindVariantField(const std::string& inName) const;
        EFunc() VariantFieldMap& GetVariantFields();
        EFunc() const VariantFieldMap& GetVariantFields() const;
        EFunc() VariantField& EmplaceVariantField(const std::string& inName);

        EFunc() bool HasParameterField(const std::string& inName) const;
        EFunc() ParameterField& GetParameterField(const std::string& inName);
        EFunc() const ParameterField& GetParameterField(const std::string& inName) const;
        EFunc() ParameterField* FindParameterField(const std::string& inName);
        EFunc() const ParameterField* FindParameterField(const std::string& inName) const;
        EFunc() ParameterFieldMap& GetParameterFields();
        EFunc() const ParameterFieldMap& GetParameterFields() const;
        EFunc() ParameterField& EmplaceParameterField(const std::string& inName);

        EFunc() void Update();

    private:
        using StageShaderTypeMap = std::unordered_map<RHI::ShaderStageBits, Common::UniquePtr<Render::MaterialShaderType>>;

        EProperty() MaterialType type;
        EProperty() std::string source;
        EProperty() VariantFieldMap variantFields;
        EProperty() ParameterFieldMap parameterFields;

        std::unordered_map<Render::VertexFactoryTypeKey, StageShaderTypeMap> shaderTypes;
    };

    class RUNTIME_API EClass() MaterialInstance final : public Asset {
        EPolyClassBody(MaterialInstance)

    public:
        using Variant = std::variant<
            bool,
            uint8_t
        >;
        using VariantMap = std::unordered_map<std::string, Variant>;

        using Parameter = std::variant<
            bool,
            int32_t,
            float,
            Common::FVec2,
            Common::FVec3,
            Common::FVec4,
            Common::FMat4x4,
            AssetPtr<Texture>,
            AssetPtr<RenderTarget>
        >;
        using ParameterMap = std::unordered_map<std::string, Parameter>;

        explicit MaterialInstance(Core::Uri inUri);

        EFunc() const AssetPtr<Material>& GetMaterial() const;
        EFunc() void SetMaterial(const AssetPtr<Material>& inMaterial);

        EFunc() bool HasVariant(const std::string& inName) const;
        EFunc() Variant GetVariant(const std::string& inName) const;
        EFunc() void SetVariant(const std::string& inName, const Variant& inVariant);
        EFunc() VariantMap GetVariants() const;

        EFunc() bool HasParameter(const std::string& inName) const;
        EFunc() Parameter GetParameter(const std::string& inName) const;
        EFunc() void SetParameter(const std::string& inName, const Parameter& inParameter);
        EFunc() ParameterMap GetParameters() const;

    private:
        EProperty() AssetPtr<Material> material;
        EProperty() VariantMap variants;
        EProperty() ParameterMap parameters;
    };
}
