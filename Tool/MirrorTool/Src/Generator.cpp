//
// Created by johnk on 2022/11/24.
//

#include <format>
#include <sstream>
#include <filesystem>

#include <MirrorTool/Generator.h>
#include <Common/Hash.h>
#include <Common/Format.h>

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
        std::stringstream stream;
        const std::string fullName = GetFullName(clazz);
        stream << std::endl << std::endl << Tab<1>() << "Mirror::Registry::Get()";
        stream << std::endl << Tab<2>() << fmt::format(R"(.Class<{}>("{}"))", fullName, fullName);
        stream << GetMetaDataCode<3>(clazz);
        for (const auto& staticVariable : clazz.staticVariables) {
            const std::string variableName = GetFullName(staticVariable);
            stream << std::endl << Tab<3>() << fmt::format(R"(.StaticVariable<&{}>("{}"))", variableName, variableName);
            stream << GetMetaDataCode<4>(staticVariable);
        }
        for (const auto& staticFunction : clazz.staticFunctions) {
            const std::string functionName = GetFullName(staticFunction);
            stream << std::endl << Tab<3>() << fmt::format(R"(.StaticFunction<&{}>("{}"))", functionName, functionName);
            stream << GetMetaDataCode<4>(staticFunction);
        }
        for (const auto& variable : clazz.variables) {
            const std::string variableName = GetFullName(variable);
            stream << std::endl << Tab<3>() << fmt::format(R"(.MemberVariable<&{}>("{}"))", variableName, variableName);
            stream << GetMetaDataCode<4>(variable);
        }
        for (const auto& function : clazz.functions) {
            const std::string functionName = GetFullName(function);
            stream << std::endl << Tab<3>() << fmt::format(R"(.MemberFunction<&{}>("{}"))", functionName, functionName);
            stream << GetMetaDataCode<4>(function);
        }
        stream << ";";
        return stream.str();
    }

    template <bool EndlOnStart>
    static std::string GetNamespaceCode(const NamespaceInfo& ns) // NOLINT
    {
        std::stringstream stream;
        if (EndlOnStart) {
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

        for (const auto& clazz : ns.classes) {
            stream << GetClassCode(clazz);
        }
        for (const auto& childNs : ns.namespaces) {
            stream << GetNamespaceCode<true>(childNs);
        }
        return stream.str();
    }

    static std::string GetMetaCode(const MetaInfo& meta)
    {
        std::stringstream stream;
        stream << GetNamespaceCode<false>(meta.global);
        for (const auto& ns : meta.namespaces) {
            stream << GetNamespaceCode<true>(ns);
        }
        return stream.str();
    }
}

namespace MirrorTool {
    Generator::Generator(std::string inOutputFile, std::string inHeaderFile, const MetaInfo& inMetaInfo)
        : outputFile(std::move(inOutputFile)), headerFile(std::move(inHeaderFile)), metaInfo(inMetaInfo) {}

    Generator::~Generator() = default;

    Generator::Result Generator::Generate()
    {
        std::filesystem::path filePath(outputFile);
        std::filesystem::path parentPath = filePath.parent_path();
        if (!std::filesystem::exists(parentPath)) {
            std::filesystem::create_directory(parentPath);
        }

        std::ofstream file(outputFile);
        if (file.fail()) {
            return std::make_pair(false, "failed to open output file");
        }

        GenerateCode(file);
        file.close();
        return std::make_pair(true, "");
    }

    void Generator::GenerateCode(std::ofstream& file)
    {
        size_t hash = Common::HashUtils::CityHash(headerFile.data(), headerFile.size());
        std::string registryName = fmt::format("_MirrorRegistry_{}", hash);

        file << "/* Generated by mirror tool, do not modify this file anyway. */" << std::endl;
        file << std::endl;
        file << fmt::format("#include <{}>", headerFile) << std::endl;
        file << "#include <Mirror/Registry.h>" << std::endl;
        file << "using namespace Mirror;" << std::endl;
        file << std::endl;
        file << "struct " << registryName << " {" << std::endl;
        file << Tab<1>() << fmt::format("{}();", registryName) << std::endl;
        file << "};" << std::endl;
        file << std::endl;
        file << fmt::format("{}::{}()", registryName, registryName) << std::endl;
        file << "{" << std::endl;
        file << GetMetaCode(metaInfo);
        file << std::endl;
        file << "}" << std::endl;
        file << std::endl;
        file << fmt::format("static {} _registry;", registryName) << std::endl;
    }
}
