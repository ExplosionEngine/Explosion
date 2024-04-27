//
// Created by johnk on 2022/7/24.
//

#include <Render/Shader.h>

namespace Render {
    ShaderReflectionData::ShaderReflectionData() = default;

    ShaderReflectionData::ShaderReflectionData(const ShaderReflectionData& inOther)
        : vertexBindings(inOther.vertexBindings)
        , resourceBindings(inOther.resourceBindings)
    {
    }

    ShaderReflectionData::ShaderReflectionData(ShaderReflectionData&& inOther) noexcept
        : vertexBindings(std::move(inOther.vertexBindings))
        , resourceBindings(std::move(inOther.resourceBindings))
    {
    }

    ShaderReflectionData& ShaderReflectionData::operator=(const ShaderReflectionData& inOther)
    {
        vertexBindings = inOther.vertexBindings;
        resourceBindings = inOther.resourceBindings;
        return *this;
    }

    const RHI::PlatformVertexBinding& ShaderReflectionData::QueryVertexBindingChecked(const ShaderReflectionData::VertexSemantic& inSemantic) const
    {
        auto iter = vertexBindings.find(inSemantic);
        Assert(iter != vertexBindings.end());
        return iter->second;
    }

    const ShaderReflectionData::LayoutAndResourceBinding& ShaderReflectionData::QueryResourceBindingChecked(const ShaderReflectionData::ResourceBindingName& inName) const
    {
        auto iter = resourceBindings.find(inName);
        Assert(iter != resourceBindings.end());
        return iter->second;
    }

    ShaderArchiveStorage& ShaderArchiveStorage::Get()
    {
        static ShaderArchiveStorage instance;
        return instance;
    }

    ShaderArchiveStorage::ShaderArchiveStorage() = default;

    ShaderArchiveStorage::~ShaderArchiveStorage() = default;

    void ShaderArchiveStorage::UpdateShaderArchivePackage(ShaderTypeKey shaderTypeKey, ShaderArchivePackage&& shaderArchivePackage)
    {
        Assert(!shaderArchivePackages.contains(shaderTypeKey));
        shaderArchivePackages.emplace(std::make_pair(shaderTypeKey, std::move(shaderArchivePackage)));
    }

    const ShaderArchivePackage& ShaderArchiveStorage::GetShaderArchivePackage(ShaderTypeKey shaderTypeKey)
    {
        auto iter = shaderArchivePackages.find(shaderTypeKey);
        Assert(iter != shaderArchivePackages.end());
        return iter->second;
    }

    void ShaderArchiveStorage::InvalidateAll()
    {
        shaderArchivePackages.clear();
    }

    void ShaderArchiveStorage::Invalidate(ShaderTypeKey shaderTypeKey)
    {
        shaderArchivePackages.erase(shaderTypeKey);
    }

    bool ShaderInstance::IsValid() const
    {
        return rhiHandle != nullptr;
    }

    size_t ShaderInstance::Hash() const
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

    GlobalShaderRegistry& GlobalShaderRegistry::Get()
    {
        static GlobalShaderRegistry instance;
        return instance;
    }

    GlobalShaderRegistry::GlobalShaderRegistry() = default;

    GlobalShaderRegistry::~GlobalShaderRegistry() = default;

    const std::vector<IShaderType*>& GlobalShaderRegistry::GetShaderTypes()
    {
        return shaderTypes;
    }

    BoolShaderVariantFieldImpl::BoolShaderVariantFieldImpl()
        : value(static_cast<uint32_t>(defaultValue))
    {
    }

    BoolShaderVariantFieldImpl::BoolShaderVariantFieldImpl(BoolShaderVariantFieldImpl&& other) noexcept
        : value(other.value)
    {
    }

    BoolShaderVariantFieldImpl::~BoolShaderVariantFieldImpl() = default;

    void BoolShaderVariantFieldImpl::Set(BoolShaderVariantFieldImpl::ValueType inValue)
    {
        value = inValue ? 1 : 0;
    }

    BoolShaderVariantFieldImpl::ValueType BoolShaderVariantFieldImpl::Get() const
    {
        return value == 1;
    }

    uint32_t BoolShaderVariantFieldImpl::GetNumberValue() const
    {
        return value;
    }
}
