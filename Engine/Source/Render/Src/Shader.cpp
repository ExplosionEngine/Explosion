//
// Created by johnk on 2022/7/24.
//

#include <Render/Shader.h>

namespace Render {
    ShaderReflectionData::ShaderReflectionData() = default;

    ShaderReflectionData::ShaderReflectionData(ShaderReflectionData&& inOther)
        : resourceBindings(std::move(inOther.resourceBindings))
    {
    }

    ShaderArchiveStorage& ShaderArchiveStorage::Get()
    {
        static ShaderArchiveStorage instance;
        return instance;
    }

    ShaderArchiveStorage::ShaderArchiveStorage() = default;

    ShaderArchiveStorage::~ShaderArchiveStorage() = default;

    void ShaderArchiveStorage::UpdateShaderArchivePackage(IShaderType* shaderTypeKey, ShaderArchivePackage&& shaderArchivePackage)
    {
        Assert(!shaderArchivePackages.contains(shaderTypeKey));
        shaderArchivePackages.emplace(std::make_pair(shaderTypeKey, std::move(shaderArchivePackage)));
    }

    const ShaderArchivePackage& ShaderArchiveStorage::GetShaderArchivePackage(IShaderType* shaderTypeKey)
    {
        auto iter = shaderArchivePackages.find(shaderTypeKey);
        Assert(iter != shaderArchivePackages.end());
        return iter->second;
    }

    void ShaderArchiveStorage::InvalidateAll()
    {
        shaderArchivePackages.clear();
    }

    void ShaderArchiveStorage::Invalidate(IShaderType* shaderTypeKey)
    {
        shaderArchivePackages.erase(shaderTypeKey);
    }
}
