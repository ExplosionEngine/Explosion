//
// Created by johnk on 2025/3/21.
//

#include <Runtime/Asset/Material.h>

namespace Runtime {
    MaterialBoolVariantField::MaterialBoolVariantField()
        : defaultValue(false)
        , sortPriority(0)
    {
    }

    MaterialRangedUintVariantField::MaterialRangedUintVariantField()
        : defaultValue(0)
        , range(0, 0)
        , sortPriority(0)
    {
    }

    MaterialBoolParameterField::MaterialBoolParameterField()
        : defaultValue(false)
        , sortPriority(0)
    {
    }

    MaterialIntParameterField::MaterialIntParameterField()
        : defaultValue(0)
        , sortPriority(0)
    {
    }

    MaterialFloatParameterField::MaterialFloatParameterField()
        : defaultValue(0.0f)
        , sortPriority(0)
    {
    }

    MaterialFVec2ParameterField::MaterialFVec2ParameterField()
        : defaultValue(Common::FVec2Consts::zero)
        , sortPriority(0)
    {
    }

    MaterialFVec3ParameterField::MaterialFVec3ParameterField()
        : defaultValue(Common::FVec3Consts::zero)
        , sortPriority(0)
    {
    }

    MaterialFVec4ParameterField::MaterialFVec4ParameterField()
        : defaultValue(Common::FVec4Consts::zero)
        , sortPriority(0)
    {
    }

    MaterialFMat4x4ParameterField::MaterialFMat4x4ParameterField()
        : defaultValue(Common::FMat4x4Consts::identity)
        , sortPriority(0)
    {
    }

    MaterialTextureParameterField::MaterialTextureParameterField()
        : sortPriority(0)
    {
    }

    MaterialRenderTargetParameterField::MaterialRenderTargetParameterField()
        : sortPriority(0)
    {
    }

    Material::Material(Core::Uri inUri)
        : Asset(std::move(inUri))
        , type(MaterialType::max)
    {
    }

    MaterialType Material::GetType() const
    {
        return type;
    }

    void Material::SetType(MaterialType inType)
    {
        type = inType;
    }

    const std::string& Material::GetSource() const
    {
        return source;
    }

    void Material::SetSource(const std::string& inSource)
    {
        source = inSource;
    }

    bool Material::HasVariantField(const std::string& inName) const
    {
        return variantFields.contains(inName);
    }

    Material::VariantField& Material::GetVariantField(const std::string& inName)
    {
        return variantFields.at(inName);
    }

    const Material::VariantField& Material::GetVariantField(const std::string& inName) const
    {
        return variantFields.at(inName);
    }

    Material::VariantField* Material::FindVariantField(const std::string& inName)
    {
        return HasVariantField(inName) ? &variantFields.at(inName) : nullptr;
    }

    const Material::VariantField* Material::FindVariantField(const std::string& inName) const
    {
        return HasVariantField(inName) ? &variantFields.at(inName) : nullptr;
    }

    Material::VariantFieldMap& Material::GetVariantFields()
    {
        return variantFields;
    }

    const Material::VariantFieldMap& Material::GetVariantFields() const
    {
        return variantFields;
    }

    Material::VariantField& Material::EmplaceVariantField(const std::string& inName)
    {
        variantFields.emplace(inName, Material::VariantField {});
        return variantFields.at(inName);
    }

    bool Material::HasParameterField(const std::string& inName) const
    {
        return parameterFields.contains(inName);
    }

    Material::ParameterField& Material::GetParameterField(const std::string& inName)
    {
        return parameterFields.at(inName);
    }

    const Material::ParameterField& Material::GetParameterField(const std::string& inName) const
    {
        return parameterFields.at(inName);
    }

    Material::ParameterField* Material::FindParameterField(const std::string& inName)
    {
        return HasParameterField(inName) ? &parameterFields.at(inName) : nullptr;
    }

    const Material::ParameterField* Material::FindParameterField(const std::string& inName) const
    {
        return HasParameterField(inName) ? &parameterFields.at(inName) : nullptr;
    }

    Material::ParameterFieldMap& Material::GetParameterFields()
    {
        return parameterFields;
    }

    const Material::ParameterFieldMap& Material::GetParameterFields() const
    {
        return parameterFields;
    }

    Material::ParameterField& Material::EmplaceParameterField(const std::string& inName)
    {
        parameterFields.emplace(inName, Material::ParameterField {});
        return parameterFields.at(inName);
    }

    MaterialInstance::MaterialInstance(Core::Uri inUri)
        : Asset(std::move(inUri))
    {
    }

    const AssetPtr<Material>& MaterialInstance::GetMaterial() const
    {
        return material;
    }

    void MaterialInstance::SetMaterial(const AssetPtr<Material>& inMaterial)
    {
        material = inMaterial;
    }

    bool MaterialInstance::HasVariant(const std::string& inName) const
    {
        return material->HasVariantField(inName);
    }

    MaterialInstance::Variant MaterialInstance::GetVariant(const std::string& inName) const
    {
        if (variants.contains(inName)) {
            return variants.at(inName);
        }

        Variant result;
        std::visit([&](auto&& variantField) -> void {
            result = variantField.defaultValue;
        }, material->GetVariantField(inName));
        return result;
    }

    void MaterialInstance::SetVariant(const std::string& inName, const Variant& inVariant)
    {
        Assert(inVariant.index() == material->GetVariantField(inName).index());
        variants[inName] = inVariant;
    }

    MaterialInstance::VariantMap MaterialInstance::GetVariants() const
    {
        VariantMap result;
        const auto& variantFields = material->GetVariantFields();
        result.reserve(variantFields.size());

        for (const auto& [name, variantField] : variantFields) {
            if (variants.contains(name)) {
                result.emplace(name, variants.at(name));
            } else {
                std::visit([&](auto&& field) -> void {
                    result.emplace(name, field.defaultValue);
                }, variantField);
            }
        }
        return result;
    }

    bool MaterialInstance::HasParameter(const std::string& inName) const
    {
        return material->HasParameterField(inName);
    }

    MaterialInstance::Parameter MaterialInstance::GetParameter(const std::string& inName) const
    {
        if (parameters.contains(inName)) {
            return parameters.at(inName);
        }

        Parameter result;
        std::visit([&](auto&& parameterField) -> void {
            result = parameterField.defaultValue;
        }, material->GetParameterField(inName));
        return result;
    }

    void MaterialInstance::SetParameter(const std::string& inName, const Parameter& inParameter)
    {
        Assert(inParameter.index() == material->GetParameterField(inName).index());
        parameters[inName] = inParameter;
    }

    MaterialInstance::ParameterMap MaterialInstance::GetParameters() const
    {
        ParameterMap result;
        const auto& parameterFields = material->GetParameterFields();
        result.reserve(parameterFields.size());

        for (const auto& [name, parameterField] : parameterFields) {
            if (parameters.contains(name)) {
                result.emplace(name, parameters.at(name));
            } else {
                std::visit([&](auto&& parameter) -> void {
                    result.emplace(name, parameter.defaultValue);
                }, parameterField);
            }
        }
        return result;
    }
} // namespace Runtime
