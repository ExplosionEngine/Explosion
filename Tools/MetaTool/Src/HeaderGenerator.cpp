//
// Created by johnk on 2022/5/25.
//

#include <sstream>
#include <filesystem>
#include <unordered_map>
#include <any>

#include <MetaTool/HeaderGenerator.h>
#include <MetaTool/ClangParser.h>
#include <Common/Debug.h>
#include <Common/String.h>

namespace MetaTool {
    enum class MetaDataType {
        BOOL,
        INTEGER,
        FLOATING,
        STRING,
        MAX
    };

    enum class ContextType {
        CLASS,
        PROPERTY,
        FUNCTION,
        MAX
    };

    template <typename T>
    struct ContextTraits {
    };

    template <>
    struct ContextTraits<ClassContext> {
        static const ContextType type = ContextType::CLASS;
    };

    template <>
    struct ContextTraits<VariableContext> {
        static const ContextType type = ContextType::PROPERTY;
    };

    template <>
    struct ContextTraits<FunctionContext> {
        static const ContextType type = ContextType::FUNCTION;
    };
    
    using MetaDataMap = std::unordered_map<std::string, std::pair<MetaDataType, std::any>>;

    void ParseMetaDataGroup(MetaDataMap& result, const std::string& prefix, const std::string& metaData);

    template <typename T>
    std::string GetContextFullName(const std::string& prefix, const T& context)
    {
        return prefix.empty() ? context.name : (prefix + "::" + context.name);
    }

    std::string GetMetaDataFullName(const std::string& prefix, const std::string& metaData)
    {
        return prefix.empty() ? metaData : (prefix + "." + metaData);
    }
    
    void ParseSingleMetaData(MetaDataMap& result, const std::string& prefix, const std::string& metaData)
    {
        if (Common::StringUtils::RegexMatch(metaData, R"(.+\(.+\))")) {
            auto name = metaData.substr(0, metaData.find('('));
            auto temp = Common::StringUtils::RegexSearchFirst(metaData, R"(\(.+\))");
            ParseMetaDataGroup(result, GetMetaDataFullName(prefix, name), temp.substr(1, temp.size() - 2));
            return;
        }

        if (Common::StringUtils::RegexMatch(metaData, R"(.+=.+)")) {
            auto splits = Common::StringUtils::Split(metaData, "=");
            Assert(splits.size() == 2);
            auto fullName = GetMetaDataFullName(prefix, splits[0]);
            if (Common::StringUtils::RegexMatch(splits[1], R"(\".+\")")) {
                result[fullName] = { MetaDataType::STRING, splits[1].substr(1, splits[1].size() - 2) };
            } else if (Common::StringUtils::RegexMatch(splits[1], R"(\-{0,1}\d+)")) {
                result[fullName] = { MetaDataType::INTEGER, std::stoi(splits[1]) };
            } else if (Common::StringUtils::RegexMatch(splits[2], R"(\-{0,1}\d+\.\d+)")) {
                result[fullName] = { MetaDataType::FLOATING, std::stof(splits[1]) };
            }
        } else {
            result[GetMetaDataFullName(prefix, metaData)] = { MetaDataType::BOOL, true };
        }
    }

    void ParseMetaDataGroup(MetaDataMap& result, const std::string& prefix, const std::string& metaData)
    {
        auto splits = Common::StringUtils::Split(metaData, ",");
        for (const auto& split : splits) {
            ParseSingleMetaData(result, prefix, split);
        }
    }

    void ParseMetaDatas(MetaDataMap& result, const std::string& name, const std::string& metaData)
    {
        result["Name"] = { MetaDataType::STRING, name };
        ParseMetaDataGroup(result, "", Common::StringUtils::Replace(metaData, " ", ""));
    }
    
    template <typename Context>
    void VerifyMetaDataTag(Context&& context, const MetaDataMap& metaDatas)
    {
        static const std::unordered_map<ContextType, std::string> stringToFindMap = {
            { ContextType::CLASS, "Class" },
            { ContextType::PROPERTY, "Property" },
            { ContextType::FUNCTION, "Function" }
        };
        auto iter = stringToFindMap.find(ContextTraits<std::remove_cvref_t<Context>>::type);
        Assert(iter != stringToFindMap.end());
        
        auto iter2 = metaDatas.find(iter->second);
        Assert(iter2 != metaDatas.end() && iter2->second.first == MetaDataType::BOOL && std::any_cast<bool>(iter2->second.second));
    }
    
    template <typename Context>
    std::string GetMetaDatasCode(Context&& context)
    {
        MetaDataMap metaDatas;
        ParseMetaDatas(metaDatas, context.name, context.metaData);
        VerifyMetaDataTag(std::forward<Context>(context), metaDatas);

        std::stringstream stream;
        for (const auto& iter : metaDatas) {
            const auto& pair = iter.second;
            switch (pair.first) {
                case MetaDataType::BOOL: {
                    stream << ", std::make_pair<size_t, bool>(hash(\"" << iter.first << "\"), " << (any_cast<bool>(pair.second) ? "true" : "false") << ")";
                    break;
                }
                case MetaDataType::INTEGER: {
                    stream << ", std::make_pair<size_t, int32_t>(hash(\"" << iter.first << "\"), " << std::to_string(any_cast<int32_t>(pair.second)) << ")";
                    break;
                }
                case MetaDataType::FLOATING: {
                    stream << ", std::make_pair<size_t, float>(hash(\"" << iter.first << "\"), " << std::to_string(any_cast<float>(pair.second)) << ")";
                    break;
                }
                case MetaDataType::STRING: {
                    stream << ", std::make_pair<size_t, std::string_view>(hash(\"" << iter.first << "\"), " << "\"" << *any_cast<std::string>(&pair.second) << "\")";
                    break;
                }
                default: break;
            }
        }
        return stream.str();
    }

    std::string CombineParameterTypes(const std::vector<ParamContext>& params)
    {
        std::stringstream stream;
        for (auto i = 0; i < params.size(); i++) {
            if (i == 0) {
                stream << params[i].type;
            } else {
                stream << ", " << params[i].type;
            }
        }
        return stream.str();
    }
}

namespace MetaTool {
    HeaderGenerator::HeaderGenerator(const HeaderGeneratorInfo& info)
        : info(info)
    {
        std::filesystem::path path(info.outputFilePath);
        std::filesystem::path targetDirPath = path.parent_path();
        if (!std::filesystem::exists(targetDirPath)) {
            std::filesystem::create_directories(targetDirPath);
        }

        file = std::ofstream(info.outputFilePath);
        Assert(file.is_open());
    }

    HeaderGenerator::~HeaderGenerator()
    {
        file.close();
    }

    void HeaderGenerator::Generate(const MetaContext& metaInfo)
    {
        GenerateFileHeader();
        GenerateIncludes();
        GenerateRegistry(metaInfo);
    }

    void HeaderGenerator::GenerateFileHeader()
    {
        file << "/**" << std::endl;
        file << " * Generated by Explosion header generator, do not modify this file anyway." << std::endl;
        file << " */" << std::endl;
        file << std::endl;
        file << "#pragma once" << std::endl;
        file << std::endl;
    }

    void HeaderGenerator::GenerateIncludes()
    {
        file << "#include <string_view>" << std::endl;
        file << "#include <utility>" << std::endl;
        file << std::endl;
        file << "#include <meta/factory.hpp>" << std::endl;
        file << std::endl;
        file << "#include <" << info.sourceFileShortPath << ">" << std::endl;
        file << std::endl;
    }

    void HeaderGenerator::GenerateRegistry(const MetaTool::MetaContext& metaInfo)
    {
        std::hash<std::string> hash {};

        file << "static int _registry_" << hash(info.outputFilePath) << " = []() -> int {" << std::endl;
        file << "    std::hash<std::string_view> hash {};" << std::endl;
        file << std::endl;
        GenerateCodeForNamespace(metaInfo.name, metaInfo);
        file << "    return 0;" << std::endl;
        file << "}();" << std::endl;
        file << std::endl;
    }

    void HeaderGenerator::GenerateCodeForNamespace(const std::string& prefix, const MetaTool::NamespaceContext& namespaceContext)
    {
        std::string fullName = GetContextFullName(prefix, namespaceContext);
        for (const auto& n : namespaceContext.namespaces) {
            GenerateCodeForNamespace(fullName, n);
        }
        for (const auto& c : namespaceContext.classes) {
            GenerateCodeForClasses(fullName, c);
        }
    }

    void HeaderGenerator::GenerateCodeForClasses(const std::string& prefix, const MetaTool::ClassContext& classContext)
    {
        std::string fullName = GetContextFullName(prefix, classContext);
        file << "    meta::reflect<" << fullName << ">(hash(\"" << fullName << "\")" << GetMetaDatasCode(classContext) << ")";
        for (const auto& c : classContext.constructors) {
            GenerateCodeForConstructor(c);
        }
        for (const auto& v : classContext.variables) {
            GenerateCodeForProperty(fullName, v);
        }
        for (const auto& f : classContext.functions) {
            GenerateCodeForFunction(fullName, f);
        }
        file << ";" << std::endl;
        file << std::endl;
    }

    void HeaderGenerator::GenerateCodeForConstructor(const MetaTool::FunctionContext& functionContext)
    {
        file << std::endl;
        file << "        .ctor<" << CombineParameterTypes(functionContext.params) << ">()";
    }

    void HeaderGenerator::GenerateCodeForProperty(const std::string& prefix, const VariableContext& variableContext)
    {
        std::string fullName = GetContextFullName(prefix, variableContext);
        file << std::endl;
        file << "        .data<&" << fullName << ">(hash(\"" << variableContext.name << "\")" << GetMetaDatasCode(variableContext) << ")";
    }

    void HeaderGenerator::GenerateCodeForFunction(const std::string& prefix, const FunctionContext& functionContext)
    {
        std::string fullName = GetContextFullName(prefix, functionContext);
        file << std::endl;
        file << "        .func<&" << fullName << ">(hash(\"" << functionContext.name << "\")" << GetMetaDatasCode(functionContext) << ")";
    }
}
