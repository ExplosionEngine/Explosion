//
// Created by johnk on 2022/7/24.
//

#include <ranges>

#include <Render/Shader.h>
#include <Common/Container.h>
#include <Core/Paths.h>
#include <Core/Thread.h>

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

    bool VertexFactoryInput::operator==(const VertexFactoryInput& inRhs) const
    {
        return name == inRhs.name
            && format == inRhs.format
            && offset == inRhs.offset;
    }

    VertexFactoryType::VertexFactoryType(VertexFactoryTypeKey inKey)
        : key(inKey)
    {
        VertexFactoryTypeRegistry::Get().RegisterType(*this);
    }

    VertexFactoryType::~VertexFactoryType()
    {
        VertexFactoryTypeRegistry::Get().UnregisterType(*this);
    }

    VertexFactoryTypeKey VertexFactoryType::GetKey() const
    {
        return key;
    }

    VertexFactoryTypeRegistry& VertexFactoryTypeRegistry::Get()
    {
        static VertexFactoryTypeRegistry instance;
        return instance;
    }

    VertexFactoryTypeRegistry::VertexFactoryTypeRegistry() = default;

    VertexFactoryTypeRegistry::~VertexFactoryTypeRegistry() = default;

    void VertexFactoryTypeRegistry::RegisterType(const VertexFactoryType& inType)
    {
        Assert(Core::ThreadContext::IsGameThread());

        const auto typeKey = inType.GetKey();
        Assert(!types.contains(typeKey));
        types.emplace(typeKey, &inType);
    }

    void VertexFactoryTypeRegistry::UnregisterType(const VertexFactoryType& inType)
    {
        Assert(Core::ThreadContext::IsGameThread());

        const auto typeKey = inType.GetKey();
        Assert(types.contains(typeKey));
        types.erase(typeKey);
    }

    std::vector<const VertexFactoryType*> VertexFactoryTypeRegistry::AllTypes() const
    {
        Assert(Core::ThreadContext::IsGameThread());

        std::vector<const VertexFactoryType*> result;
        result.reserve(types.size());
        for (const auto* type : types | std::views::values) {
            result.emplace_back(type);
        }
        return result;
    }

    ShaderType::ShaderType(ShaderTypeKey inKey)
        : key(inKey)
    {
        ShaderTypeRegistry::Get().RegisterType(*this);
    }

    ShaderType::~ShaderType()
    {
        ShaderTypeRegistry::Get().UnregisterType(*this);
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
        : ShaderType(Internal::MakeTypeKeyFromName(inName))
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

    ShaderTypeRegistry& ShaderTypeRegistry::Get()
    {
        static ShaderTypeRegistry registry;
        return registry;
    }

    ShaderTypeRegistry::ShaderTypeRegistry() = default;

    ShaderTypeRegistry::~ShaderTypeRegistry() = default;

    void ShaderTypeRegistry::RegisterType(const ShaderType& inShaderType)
    {
        Assert(Core::ThreadContext::IsGameThread());

        const auto key = inShaderType.GetKey();
        Assert(!types.contains(key));
        types.emplace(key, &inShaderType);
    }

    void ShaderTypeRegistry::UnregisterType(const ShaderType& inShaderType)
    {
        Assert(Core::ThreadContext::IsGameThread());

        const auto key = inShaderType.GetKey();
        Assert(types.contains(key));
        types.erase(key);
    }

    const ShaderType& ShaderTypeRegistry::GetType(ShaderTypeKey inKey) const
    {
        Assert(Core::ThreadContext::IsGameThread());
        return *types.at(inKey);
    }

    std::vector<const ShaderType*> ShaderTypeRegistry::AllTypes() const
    {
        Assert(Core::ThreadContext::IsGameThread());

        std::vector<const ShaderType*> result;
        result.reserve(types.size());
        for (const auto* type : types | std::views::values) {
            result.emplace_back(type);
        }
        return result;
    }

    ShaderArtifactRegistry& ShaderArtifactRegistry::Get()
    {
        static ShaderArtifactRegistry instance;
        return instance;
    }

    ShaderArtifactRegistry::ShaderArtifactRegistry() = default;

    ShaderArtifactRegistry::~ShaderArtifactRegistry() = default;

    void ShaderArtifactRegistry::PerformThreadCopy()
    {
        typeArtifactsRT = typeArtifactsGT;
    }

    ShaderMap& ShaderMap::Get(RHI::Device& inDevice)
    {
        static std::unordered_map<RHI::Device*, Common::UniquePtr<ShaderMap>> instances;
        if (!instances.contains(&inDevice)) {
            instances.emplace(&inDevice, Common::UniquePtr(new ShaderMap(inDevice)));
        }
        return *instances.at(&inDevice);
    }

    ShaderMap::ShaderMap(RHI::Device& inDevice)
        : device(inDevice)
    {
    }

    ShaderMap::~ShaderMap() = default;

    ShaderInstance ShaderMap::GetShaderInstance(const ShaderType& inShaderType, const ShaderVariantValueMap& inShaderVariants)
    {
        Assert(Core::ThreadContext::IsRenderThread());

        const ShaderArtifactRegistry& registry = ShaderArtifactRegistry::Get();
        const auto typeKey = inShaderType.GetKey();

        const ShaderTypeArtifact& typeArtifact = registry.typeArtifactsRT.at(typeKey); // NOLINT
        const ShaderVariantKey variantKey = ShaderUtils::ComputeVariantKey(inShaderType.GetVariantFields(), inShaderVariants);
        const auto& [entryPoint, byteCode, reflectionData] = typeArtifact.variantArtifacts.at(variantKey);

        if (!shaderModules.contains(typeKey)) {
            shaderModules.emplace(typeKey, VariantsShaderModules {});
        }
        VariantsShaderModules& variantShaderModules = shaderModules.at(typeKey);
        if (!variantShaderModules.contains(variantKey)) {
            variantShaderModules.emplace(variantKey, device.CreateShaderModule(RHI::ShaderModuleCreateInfo(entryPoint, byteCode)));
        }

        ShaderInstance result;
        result.typeKey = typeKey;
        result.variantKey = variantKey;
        result.rhiHandle = variantShaderModules.at(variantKey).Get();
        result.reflectionData = &reflectionData;
        return result;
    }
} // namespace Render
