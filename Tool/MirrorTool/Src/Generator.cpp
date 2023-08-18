//
// Created by johnk on 2022/11/24.
//

#include <sstream>
#include <filesystem>
#include <utility>

#include <MirrorTool/Generator.h>
#include <Common/Hash.h>
#include <Common/String.h>

namespace MirrorTool {
    template <uint8_t N>
    struct Tab {
        template <typename S>
        friend S& operator<<(S& file, const Tab& tab)
        {
            for (auto i = 0; i < N * 4; i++) {
                file << " ";
            }
            return file;
        }
    };

    static std::string GetFullName(const Node& node)
    {
        std::string outerName = node.outerName;
        const std::string name = node.name;
        return outerName.empty() ? name : fmt::format("{}::{}", outerName, name);
    }

    template <uint8_t TabN>
    static std::string GetMetaDataCode(const Node& node)
    {
        std::stringstream stream;
        for (const auto& metaData : node.metaDatas) {
            stream << std::endl << Tab<TabN>() << fmt::format(R"(.MetaData("{}", "{}"))", metaData.first, metaData.second);
        }
        return stream.str();
    }

    static std::string GetClassCode(const ClassInfo& clazz)
    {
        if (clazz.metaDatas.empty() && clazz.staticVariables.empty() && clazz.staticFunctions.empty() && clazz.variables.empty() && clazz.functions.empty()) {
            return "";
        }

        const std::string fullName = GetFullName(clazz);

        std::stringstream stream;
        stream << std::endl;
        stream << fmt::format("{}::_MirrorRegistry::_MirrorRegistry()", fullName) << std::endl;
        stream << "{" << std::endl;
        stream << Tab<1>() << "Mirror::Registry::Get()";
        stream << std::endl << Tab<2>() << fmt::format(R"(.Class<{}>("{}"))", fullName, fullName);
        stream << GetMetaDataCode<3>(clazz);
        for (const auto& constructor : clazz.constructors) {
            stream << std::endl << Tab<3>() << fmt::format(R"(.Constructor<{}>("{}"))", constructor.name, constructor.name);
            stream << GetMetaDataCode<4>(constructor);
        }
        for (const auto& staticVariable : clazz.staticVariables) {
            const std::string variableName = GetFullName(staticVariable);
            stream << std::endl << Tab<3>() << fmt::format(R"(.StaticVariable<&{}>("{}"))", variableName, staticVariable.name);
            stream << GetMetaDataCode<4>(staticVariable);
        }
        for (const auto& staticFunction : clazz.staticFunctions) {
            const std::string functionName = GetFullName(staticFunction);
            stream << std::endl << Tab<3>() << fmt::format(R"(.StaticFunction<&{}>("{}"))", functionName, staticFunction.name);
            stream << GetMetaDataCode<4>(staticFunction);
        }
        for (const auto& variable : clazz.variables) {
            const std::string variableName = GetFullName(variable);
            stream << std::endl << Tab<3>() << fmt::format(R"(.MemberVariable<&{}>("{}"))", variableName, variable.name);
            stream << GetMetaDataCode<4>(variable);
        }
        for (const auto& function : clazz.functions) {
            const std::string functionName = GetFullName(function);
            stream << std::endl << Tab<3>() << fmt::format(R"(.MemberFunction<&{}>("{}"))", functionName, function.name);
            stream << GetMetaDataCode<4>(function);
        }
        stream << ";";
        stream << std::endl;
        stream << "}" << std::endl;
        stream << std::endl;
        stream << fmt::format("{}::_MirrorRegistry {}::_mirrorRegistry;", fullName, fullName) << std::endl;
        stream << std::endl;
        return stream.str();
    }

    static std::string GetNamespaceGlobalScopeCode(const NamespaceInfo& ns, bool& firstBlock) // NOLINT
    {
        if (ns.metaDatas.empty() && ns.variables.empty() && ns.functions.empty() && ns.classes.empty() && ns.namespaces.empty()) {
            return "";
        }

        std::stringstream stream;
        if (!ns.metaDatas.empty() || !ns.metaDatas.empty()) {
            if (firstBlock) {
                firstBlock = false;
            } else {
                stream << std::endl << std::endl;
            }
            stream << Tab<1>() << "Mirror::Registry::Get()";
            stream << std::endl << Tab<2>() << ".Global()";
            stream << GetMetaDataCode<3>(ns);
            for (const auto& variable : ns.variables) {
                const std::string variableName = GetFullName(variable);
                stream << std::endl << Tab<3>() << fmt::format(R"(.Variable<&{}>("{}"))", variableName, variableName);
                stream << GetMetaDataCode<4>(variable);
            }
            for (const auto& function : ns.functions) {
                const std::string functionName = GetFullName(function);
                stream << std::endl << Tab<3>() << fmt::format(R"(.Function<&{}>("{}"))", functionName, functionName);
                stream << GetMetaDataCode<4>(function);
            }
            stream << ";";
        }

        for (const auto& childNs : ns.namespaces) {
            stream << GetNamespaceGlobalScopeCode(childNs, firstBlock);
        }
        return stream.str();
    }

    static std::string GetGlobalScopeCode(const MetaInfo& meta)
    {
        bool firstBlock = true;
        std::stringstream stream;
        stream << GetNamespaceGlobalScopeCode(meta.global, firstBlock);
        for (const auto& ns : meta.namespaces) {
            stream << GetNamespaceGlobalScopeCode(ns, firstBlock);
        }
        return stream.str();
    }

    static std::string GetBestMatchHeaderPath(const std::string& inputFile, const std::vector<std::string>& headerDirs)
    {
        for (const auto& headerDir : headerDirs) {
            if (headerDir.empty()) {
                return "";
            }
            if (inputFile.starts_with(headerDir)) {
                auto result = Common::StringUtils::Replace(inputFile, headerDir, "");
                return result.starts_with("/") ? result.substr(1) : result;
            }
        }
        return "";
    }

    static std::string GetHeaderNote()
    {
        std::stringstream stream;
        stream << "/* Generated by mirror tool, do not modify this file anyway. */" << std::endl;
        return stream.str();
    }

    static std::string GetGlobalScopeRegistryCode(const MetaInfo& metaInfo, const std::string& outputFile)
    {
        size_t hash = Common::HashUtils::CityHash(outputFile.data(), outputFile.size());
        std::string registryName = fmt::format("_GlobalScopeMirrorRegistry_{}", hash);

        std::stringstream stream;
        stream << "struct " << registryName << " {" << std::endl;
        stream << Tab<1>() << fmt::format("{}();", registryName) << std::endl;
        stream << "};" << std::endl;
        stream << std::endl;
        stream << fmt::format("{}::{}()", registryName, registryName) << std::endl;
        stream << "{" << std::endl;
        stream << GetGlobalScopeCode(metaInfo);
        stream << std::endl;
        stream << "}" << std::endl;
        stream << std::endl;
        stream << fmt::format("static {} _globalScopeRegistry;", registryName) << std::endl;
        return stream.str();
    }

    static std::string GetNamespaceClassesCode(const NamespaceInfo& ns)
    {
        std::stringstream stream;
        for (const auto& clazz : ns.classes) {
            stream << GetClassCode(clazz);
        }
        return stream.str();
    }

    static std::string GetClassesCode(const MetaInfo& metaInfo)
    {
        std::stringstream stream;
        for (const auto& clazz : metaInfo.global.classes) {
            stream << GetClassCode(clazz);
        }
        for (const auto& ns : metaInfo.namespaces) {
            stream << GetNamespaceClassesCode(ns);
        }
        return stream.str();
    }
}

namespace MirrorTool {
    Generator::Generator(std::string inInputFile, std::string inOutputFile, std::vector<std::string> inHeaderDirs, const MetaInfo& inMetaInfo)
        : inputFile(std::move(inInputFile)), outputFile(std::move(inOutputFile)), headerDirs(std::move(inHeaderDirs)), metaInfo(inMetaInfo) {}

    Generator::~Generator() = default;

    Generator::Result Generator::Generate()
    {
        std::filesystem::path filePath(outputFile);
        std::filesystem::path parentPath = filePath.parent_path();
        if (!std::filesystem::exists(parentPath)) {
            std::filesystem::create_directories(parentPath);
        }

        std::ofstream file(outputFile);
        if (file.fail()) {
            return std::make_pair(false, "failed to open output file");
        }

        auto result = GenerateCode(file);
        file.close();
        return result;
    }

    Generator::Result Generator::GenerateCode(std::ofstream& file)
    {
        std::string bestMatchHeaderPath = GetBestMatchHeaderPath(inputFile, headerDirs);
        if (bestMatchHeaderPath.empty()) {
            return std::make_pair(false, "failed to compute best match header path");
        }

        file << GetHeaderNote() << std::endl;
        file << fmt::format("#include <{}>", bestMatchHeaderPath) << std::endl;
        file << "#include <Mirror/Registry.h>" << std::endl;
        file << std::endl;
        file << GetGlobalScopeRegistryCode(metaInfo, outputFile);
        file << GetClassesCode(metaInfo);
        return std::make_pair(true, "");
    }
}
