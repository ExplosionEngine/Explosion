//
// Created by johnk on 2025/3/21.
//

#include <Runtime/Asset/Material.h>

namespace Runtime {
    MaterialParameter::MaterialParameter() = default;

    MaterialParameterType MaterialParameter::GetType() const
    {
        constexpr MaterialParameterType vec[] = {
            MaterialParameterType::max,
            MaterialParameterType::tInt,
            MaterialParameterType::tFloat,
            MaterialParameterType::tFVec2,
            MaterialParameterType::tFVec3,
            MaterialParameterType::tFVec4,
            MaterialParameterType::tFMat4x4
        };
        static_assert(sizeof(vec) / sizeof(MaterialParameterType) == std::variant_size_v<decltype(parameter)>);
        return vec[parameter.index()];
    }

    int32_t MaterialParameter::GetInt() const
    {
        Assert(GetType() == MaterialParameterType::tInt);
        return std::get<int32_t>(parameter);
    }

    float MaterialParameter::GetFloat() const
    {
        Assert(GetType() == MaterialParameterType::tFloat);
        return std::get<float>(parameter);
    }

    Common::FVec2 MaterialParameter::GetFVec2() const
    {
        Assert(GetType() == MaterialParameterType::tFVec2);
        return std::get<Common::FVec2>(parameter);
    }

    Common::FVec3 MaterialParameter::GetFVec3() const
    {
        Assert(GetType() == MaterialParameterType::tFVec3);
        return std::get<Common::FVec3>(parameter);
    }

    Common::FVec4 MaterialParameter::GetFVec4() const
    {
        Assert(GetType() == MaterialParameterType::tFVec4);
        return std::get<Common::FVec4>(parameter);
    }

    Common::FMat4x4 MaterialParameter::GetFMat4x4() const
    {
        Assert(GetType() == MaterialParameterType::tFMat4x4);
        return std::get<Common::FMat4x4>(parameter);
    }

    void MaterialParameter::SetInt(int32_t inValue)
    {
        parameter = inValue;
    }

    void MaterialParameter::SetFloat(float inValue)
    {
        parameter = inValue;
    }

    void MaterialParameter::SetFVec2(const Common::FVec2& inValue)
    {
        parameter = inValue;
    }

    void MaterialParameter::SetFVec3(const Common::FVec3& inValue)
    {
        parameter = inValue;
    }

    void MaterialParameter::SetFVec4(const Common::FVec4& inValue)
    {
        parameter = inValue;
    }

    void MaterialParameter::SetFMat4x4(const Common::FMat4x4& inValue)
    {
        parameter = inValue;
    }

    IMaterial::IMaterial(Core::Uri inUri)
        : Asset(std::move(inUri))
    {
    }

    IMaterial::~IMaterial() = default;

    Material::Material(Core::Uri inUri)
        : IMaterial(std::move(inUri))
        , type(MaterialType::max)
    {
    }

    Material::~Material() = default;

    MaterialType Material::GetType() const
    {
        return type;
    }

    std::string Material::GetSourceCode() const
    {
        return sourceCode;
    }

    void Material::SetParameter(const std::string& inName, const MaterialParameter& inParameter)
    {
        parameters[inName] = inParameter;
    }

    void Material::SetType(MaterialType inType)
    {
        type = inType;
    }

    void Material::SetSourceCode(const std::string& inSourceCode)
    {
        sourceCode = inSourceCode;
    }

    IMaterial::ParameterMap& Material::GetParameters()
    {
        return parameters;
    }

    const IMaterial::ParameterMap& Material::GetParameters() const
    {
        return parameters;
    }

    MaterialInstance::MaterialInstance(Core::Uri inUri)
        : IMaterial(std::move(inUri))
    {
    }

    MaterialInstance::~MaterialInstance() = default;

    MaterialType MaterialInstance::GetType() const
    {
        return material->GetType();
    }

    std::string MaterialInstance::GetSourceCode() const
    {
        return material->GetSourceCode();
    }

    void MaterialInstance::SetParameter(const std::string& inName, const MaterialParameter& inParameter)
    {
        parameters[inName] = inParameter;
    }

    IMaterial::ParameterMap& MaterialInstance::GetParameters()
    {
        return parameters;
    }

    const IMaterial::ParameterMap& MaterialInstance::GetParameters() const
    {
        return parameters;
    }

    AssetPtr<Material> MaterialInstance::GetMaterial() const
    {
        return material;
    }

    void MaterialInstance::SetMaterial(const AssetPtr<Material>& inMaterial) // NOLINT
    {
        material = inMaterial;
    }
}
