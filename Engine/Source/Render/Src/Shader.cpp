//
// Created by johnk on 2022/7/24.
//

#include "Core/Paths.h"

#include <ranges>

#include <Render/Shader.h>
#include <Common/Container.h>

namespace Render {
    bool ShaderBoolVariantField::operator==(const ShaderBoolVariantField& inRhs) const
    {
        return macro == inRhs.macro
            && defaultValue == inRhs.defaultValue;
    }

    bool ShaderRangedIntVariantField::operator==(const ShaderRangedIntVariantField& inRhs) const
    {
        return macro == inRhs.macro
            && defaultValue == inRhs.defaultValue
            && range == inRhs.range;
    }

    std::vector<ShaderVariantValueMap> ShaderUtils::GetAllVariants(const ShaderVariantFieldVec& inFields)
    {
        std::vector<ShaderVariantValueMap> result;

        ShaderVariantValueMap baseVariant;
        for (const auto& field : inFields) {
            std::visit([&](auto&& typedField) -> void {
                baseVariant.emplace(typedField.macro, typedField.defaultValue);
            }, field);
        }
        result.emplace_back(baseVariant);

        for (const auto& field : inFields) {
            int32_t variantFieldRange = 0;
            if (field.index() == 0) {
                variantFieldRange = 2;
            } else if (field.index() == 1) {
                const auto& [macro, defaultValue, range] = std::get<ShaderRangedIntVariantField>(field);
                variantFieldRange = range.second - range.first + 1;
            } else {
                Unimplement();
            }

            const auto fork = result;
            result.clear();
            result.reserve(fork.size() * variantFieldRange);

            if (field.index() == 0) {
                const auto& [macro, defaultValue] = std::get<ShaderBoolVariantField>(field);
                for (const std::vector<bool> candidateValues = { false, true };
                    auto candidateValue : candidateValues) {
                    for (const auto& valueMap : fork) {
                        auto& last = result.emplace_back(valueMap);
                        last.at(macro) = candidateValue;
                    }
                }
            } else if (field.index() == 1) {
                const auto& [macro, defaultValue, range] = std::get<ShaderRangedIntVariantField>(field);
                for (auto i = range.first; i <= range.second; i++) {
                    for (const auto& valueMap : fork) {
                        auto& last = result.emplace_back(valueMap);
                        last.at(macro) = i;
                    }
                }
            } else {
                Unimplement();
            }
        }
        return result;
    }

    ShaderVariantKey ShaderUtils::ComputeVariantKey(const ShaderVariantFieldVec& inFields, const ShaderVariantValueMap& inVariantSet)
    {
        uint64_t result = 0;
        uint64_t variantKeyMultipy = 1;
        for (const auto& field : inFields) {
            uint64_t variantFieldValue = 0;
            uint64_t variantFieldRange = 1;

            if (field.index() == 0) {
                const auto& [macro, defaultValue] = std::get<ShaderBoolVariantField>(field);
                const bool value = inVariantSet.contains(macro) ? std::get<bool>(inVariantSet.at(macro)) : defaultValue;
                variantFieldValue = value ? 1 : 0;
                variantFieldRange = 2;
            } else if (field.index() == 1) {
                const auto& [macro, defaultValue, range] = std::get<ShaderRangedIntVariantField>(field);
                const int32_t value = inVariantSet.contains(macro) ? std::get<int32_t>(inVariantSet.at(macro)) : defaultValue;
                variantFieldValue = value - range.first;
                variantFieldRange = range.second - range.first + 1;
            } else {
                Unimplement();
            }

            result *= variantKeyMultipy;
            result += variantFieldValue;
            variantKeyMultipy *= variantFieldRange;
        }
        return result;
    }

    std::vector<std::string> ShaderUtils::ComputeVariantDefinitions(const ShaderVariantFieldVec& inFields, const ShaderVariantValueMap& inVariantSet)
    {
        std::vector<std::string> result;
        result.reserve(inFields.size());
        for (const auto& field : inFields) {
            if (field.index() == 0) {
                const auto& [macro, defaultValue] = std::get<ShaderBoolVariantField>(field);
                const bool value = inVariantSet.contains(macro) ? std::get<bool>(inVariantSet.at(macro)) : defaultValue;
                result.emplace_back(std::format("{}={}", macro, value ? 1 : 0));
            } else if (field.index() == 1) {
                const auto& [macro, defaultValue, range] = std::get<ShaderRangedIntVariantField>(field);
                const int32_t value = inVariantSet.contains(macro) ? std::get<int32_t>(inVariantSet.at(macro)) : defaultValue;
                result.emplace_back(std::format("{}={}", macro, value));
            } else {
                Unimplement();
            }
        }
        return result;
    }

    ShaderSourceHash ShaderUtils::ComputeShaderSourceHash(const std::string& inSourceFile, const std::vector<std::string>& inIncludeDirectories)
    {
        std::unordered_map<std::string, std::string> relativeFileAndSources;
        GatherShaderSources(relativeFileAndSources, inSourceFile, inIncludeDirectories);

        std::string finalString;
        for (const auto& source : relativeFileAndSources | std::views::values) {
            finalString += source;
        }
        return Common::HashUtils::CityHash(finalString.data(), finalString.size());
    }

    std::string ShaderUtils::GetAbsoluteIncludeFile(const std::string& inPath, const std::vector<std::string>& inIncludeDirectories)
    {
        for (const auto& includeDirectory : inIncludeDirectories) {
            const Common::Path absoluteIncludeDirectory = Core::Paths::Translate(includeDirectory);
            auto testPath = absoluteIncludeDirectory / inPath;
            testPath.Fixup();

            if (testPath.Exists()) {
                return testPath.String();
            }
        }
        QuickFail();
        return "";
    }

    void ShaderUtils::GatherShaderSources(std::unordered_map<std::string, std::string>& outFileAndSource, const std::string& inSourceFile, const std::vector<std::string>& inIncludeDirectories)
    {
        const std::string text = Common::FileUtils::ReadTextFile(inSourceFile);
        outFileAndSource.emplace(inSourceFile, text);

        for (const auto includes = Common::StringUtils::RegexSearch(text, "#include \\<.*\\>");
            const auto& include : includes) {
            auto pureInclude = Common::StringUtils::Replace(include, "#include <", "");
            pureInclude = Common::StringUtils::Replace(pureInclude, ">", "");
            const std::string absoluteInclude = GetAbsoluteIncludeFile(pureInclude, inIncludeDirectories);
            GatherShaderSources(outFileAndSource, absoluteInclude, inIncludeDirectories);
        }
    }

    VertexFactoryType::~VertexFactoryType() = default;

    VertexFactoryType::VertexFactoryType() = default;

    ShaderTypeKey ShaderType::MakeTypeKeyFromName(const std::string& inName)
    {
        return Common::HashUtils::CityHash(inName.data(), inName.size());
    }

    ShaderType::ShaderType(ShaderTypeKey inKey)
        : key(inKey)
    {
        ShaderRegistry::Get().RegisterType(*this);
    }

    ShaderType::~ShaderType()
    {
        ShaderRegistry::Get().UnregisterType(*this);
    }

    ShaderTypeKey ShaderType::GetKey() const
    {
        return key;
    }

    MaterialShaderType::MaterialShaderType(
        const VertexFactoryType& inVertexFactory,
        std::string inName,
        RHI::ShaderStageBits inStage,
        std::string inSourceFile,
        std::string inEntryPoint,
        const std::vector<std::string>& inIncludeDirectories,
        const ShaderVariantFieldVec& inShaderVariantFields)
        : ShaderType(MakeTypeKeyFromName(inName))
        , vertexFactory(inVertexFactory)
        , name(std::move(inName))
        , stage(inStage)
        , sourceFile(std::move(inSourceFile))
        , entryPoint(std::move(inEntryPoint))
        , includeDirectories(inIncludeDirectories)
        , shaderVariantFields(Common::VectorUtils::Combine(inVertexFactory.GetVariantFields(), inShaderVariantFields))
    {
    }

    MaterialShaderType::~MaterialShaderType() = default;

    const std::string& MaterialShaderType::GetName() const
    {
        return name;
    }

    RHI::ShaderStageBits MaterialShaderType::GetStage() const
    {
        return stage;
    }

    const std::string& MaterialShaderType::GetSourceFile() const
    {
        return sourceFile;
    }

    const std::string& MaterialShaderType::GetEntryPoint() const
    {
        return entryPoint;
    }

    const std::vector<std::string>& MaterialShaderType::GetIncludeDirectories() const
    {
        return includeDirectories;
    }

    const ShaderVariantFieldVec& MaterialShaderType::GetVariantFields() const
    {
        return shaderVariantFields;
    }

    ShaderReflectionData::ShaderReflectionData() = default;

    ShaderReflectionData::ShaderReflectionData(const ShaderReflectionData& inOther) // NOLINT
        : vertexBindings(inOther.vertexBindings)
        , resourceBindings(inOther.resourceBindings)
    {
    }

    ShaderReflectionData::ShaderReflectionData(ShaderReflectionData&& inOther) noexcept // NOLINT
        : vertexBindings(std::move(inOther.vertexBindings))
        , resourceBindings(std::move(inOther.resourceBindings))
    {
    }

    ShaderReflectionData& ShaderReflectionData::operator=(const ShaderReflectionData& inOther) // NOLINT
    {
        vertexBindings = inOther.vertexBindings;
        resourceBindings = inOther.resourceBindings;
        return *this;
    }

    const RHI::PlatformVertexBinding& ShaderReflectionData::QueryVertexBindingChecked(const VertexSemantic& inSemantic) const
    {
        const auto iter = vertexBindings.find(inSemantic);
        Assert(iter != vertexBindings.end());
        return iter->second;
    }

    const ShaderReflectionData::LayoutAndResourceBinding& ShaderReflectionData::QueryResourceBindingChecked(const ResourceBindingName& inName) const
    {
        const auto iter = resourceBindings.find(inName);
        Assert(iter != resourceBindings.end());
        return iter->second;
    }

    ShaderInstance::ShaderInstance()
        : typeKey(0)
        , variantKey(0)
        , rhiHandle(nullptr)
        , reflectionData(nullptr)
    {
    }

    bool ShaderInstance::Valid() const
    {
        return rhiHandle != nullptr;
    }

    uint64_t ShaderInstance::Hash() const
    {
        const std::vector<uint64_t> values = {
            typeKey,
            variantKey
        };
        return Common::HashUtils::CityHash(values.data(), values.size() * sizeof(uint64_t));
    }

    ShaderRegistry& ShaderRegistry::Get()
    {
        static ShaderRegistry registry;
        return registry;
    }

    ShaderRegistry::ShaderRegistry() = default;

    ShaderRegistry::~ShaderRegistry() = default;

    void ShaderRegistry::RegisterType(const ShaderType& inShaderType)
    {
        const auto key = inShaderType.GetKey();
        Assert(!shaderStorages.contains(key));
        shaderStorages.emplace(std::make_pair(key, ShaderStorage { &inShaderType, shaderSourceHashNotCompiled }));
    }

    void ShaderRegistry::UnregisterType(const ShaderType& inShaderType)
    {
        const auto key = inShaderType.GetKey();
        Assert(shaderStorages.contains(key));
        shaderStorages.erase(key);
    }

    void ShaderRegistry::ResetType(const ShaderType& inShaderType)
    {
        shaderStorages.at(inShaderType.GetKey()) = ShaderStorage { &inShaderType, shaderSourceHashNotCompiled };
    }

    void ShaderRegistry::ResetAllTypes()
    {
        for (auto& storage : shaderStorages | std::views::values) {
            storage = ShaderStorage { storage.shaderType, shaderSourceHashNotCompiled };
        }
    }

    const ShaderType& ShaderRegistry::GetType(ShaderTypeKey inKey) const
    {
        return *shaderStorages.at(inKey).shaderType;
    }

    std::vector<const ShaderType*> ShaderRegistry::AllTypes() const
    {
        std::vector<const ShaderType*> result;
        result.reserve(shaderStorages.size());
        for (const auto& shaderStorage : shaderStorages | std::views::values) {
            result.emplace_back(shaderStorage.shaderType);
        }
        return result;
    }

    ShaderInstance ShaderRegistry::GetShaderInstance(RHI::Device& inDevice, const ShaderType& inShaderType, const ShaderVariantValueMap& inShaderVariants)
    {
        const auto typeKey = inShaderType.GetKey();
        ShaderStorage& storage = shaderStorages.at(typeKey);
        auto& shaderModuleMap = storage.deviceShaderModules[&inDevice];

        const ShaderVariantKey variantKey = ShaderUtils::ComputeVariantKey(inShaderType.GetVariantFields(), inShaderVariants);
        const auto& [entryPoint, byteCode, reflectionData] = storage.shaderModuleDatas.at(variantKey);

        ShaderInstance result;
        result.typeKey = typeKey;
        result.variantKey = variantKey;

        if (const auto iter = shaderModuleMap.find(variantKey);
            iter != shaderModuleMap.end()) {
            result.rhiHandle = iter->second.Get();
        } else {
            shaderModuleMap.emplace(variantKey, inDevice.CreateShaderModule(RHI::ShaderModuleCreateInfo(entryPoint, byteCode)));
            result.rhiHandle = shaderModuleMap.at(variantKey).Get();
        }
        result.reflectionData = &reflectionData;
        return result;
    }
}
