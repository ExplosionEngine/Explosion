//
// Created by johnk on 2022/7/24.
//

#include <Render/Shader.h>

namespace Render {
    ShaderByteCodeStorage& ShaderByteCodeStorage::Get()
    {
        static ShaderByteCodeStorage instance;
        return instance;
    }

    ShaderByteCodeStorage::ShaderByteCodeStorage() = default;

    ShaderByteCodeStorage::~ShaderByteCodeStorage() = default;

    void ShaderByteCodeStorage::UpdateByteCodePackage(IShaderType* shaderTypeKey, std::unordered_map<VariantKey, ShaderByteCode>&& byteCodePackage)
    {
        byteCodePackages[shaderTypeKey] = byteCodePackage;
    }

    const std::unordered_map<VariantKey, ShaderByteCode>& ShaderByteCodeStorage::GetByteCodePackage(IShaderType* shaderTypeKey)
    {
        auto iter = byteCodePackages.find(shaderTypeKey);
        Assert(iter != byteCodePackages.end());
        return iter->second;
    }
}
