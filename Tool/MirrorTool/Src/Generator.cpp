//
// Created by johnk on 2022/11/24.
//

#include <sstream>
#include <filesystem>
#include <utility>

#include <MirrorTool/Generator.h>
#include <Common/Hash.h>
#include <Common/String.h>
#include <Common/IO.h>

namespace MirrorTool {
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
        for (const auto& [key, value] : node.metaDatas) {
            stream << Common::newline << Common::tab<TabN> << fmt::format(R"(.MetaData("{}", "{}"))", key, value);
        }
        return stream.str();
    }

    static std::string GetClassCode(const ClassInfo& clazz) // NOLINT
    {
        const std::string fullName = GetFullName(clazz);

        std::stringstream stream;
        stream << Common::newline;
        stream << fmt::format("int {}::_mirrorRegistry = []() -> int ", fullName) << Common::newline;
        stream << "{" << Common::newline;
        stream << Common::tab<1> << "Mirror::Registry::Get()";
        if (clazz.baseClassName.empty()) {
            stream << Common::newline << Common::tab<2> << fmt::format(R"(.Class<{}>("{}"))", fullName, fullName);
        } else {
            stream << Common::newline << Common::tab<2> << fmt::format(R"(.Class<{}, {}>("{}"))", fullName, clazz.baseClassName, fullName);
        }
        stream << GetMetaDataCode<3>(clazz);
        for (const auto& constructor : clazz.constructors) {
            stream << Common::newline << Common::tab<3> << fmt::format(R"(.Constructor<{}>("{}"))", constructor.name, constructor.name);
            stream << GetMetaDataCode<4>(constructor);
        }
        for (const auto& staticVariable : clazz.staticVariables) {
            const std::string variableName = GetFullName(staticVariable);
            stream << Common::newline << Common::tab<3> << fmt::format(R"(.StaticVariable<&{}>("{}"))", variableName, staticVariable.name);
            stream << GetMetaDataCode<4>(staticVariable);
        }
        for (const auto& staticFunction : clazz.staticFunctions) {
            const std::string functionName = GetFullName(staticFunction);
            stream << Common::newline << Common::tab<3> << fmt::format(R"(.StaticFunction<&{}>("{}"))", functionName, staticFunction.name);
            stream << GetMetaDataCode<4>(staticFunction);
        }
        for (const auto& variable : clazz.variables) {
            const std::string variableName = GetFullName(variable);
            stream << Common::newline << Common::tab<3> << fmt::format(R"(.MemberVariable<&{}>("{}"))", variableName, variable.name);
            stream << GetMetaDataCode<4>(variable);
        }
        for (const auto& function : clazz.functions) {
            const std::string functionName = GetFullName(function);
            stream << Common::newline << Common::tab<3> << fmt::format(R"(.MemberFunction<&{}>("{}"))", functionName, function.name);
            stream << GetMetaDataCode<4>(function);
        }
        stream << ";" << Common::newline;
        stream << Common::tab<1> << "return 0;" << Common::newline;
        stream << "}();" << Common::newline;
        stream << Common::newline;
        stream << fmt::format("const Mirror::Class& {}::GetClass()", fullName) << Common::newline;
        stream << "{" << Common::newline;
        stream << Common::tab<1> << fmt::format("static const Mirror::Class& clazz = Mirror::Class::Get<{}>();", fullName) << Common::newline;
        stream << Common::tab<1> << "return clazz;" << Common::newline;
        stream << "}" << Common::newline;
        stream << Common::newline;

        for (const auto& internalClass : clazz.classes) {
            stream << GetClassCode(internalClass);
        }
        return stream.str();
    }

    static std::string GetNamespaceGlobalScopeCode(const NamespaceInfo& ns, bool& firstBlock) // NOLINT
    {
        if (ns.metaDatas.empty() && ns.variables.empty() && ns.functions.empty() && ns.classes.empty() && ns.namespaces.empty()) {
            return "";
        }

        std::stringstream stream;
        if (!ns.metaDatas.empty()) {
            if (firstBlock) {
                firstBlock = false;
            } else {
                stream << Common::newline << Common::newline;
            }
            stream << Common::tab<1> << "Mirror::Registry::Get()";
            stream << Common::newline << Common::tab<2> << ".Global()";
            stream << GetMetaDataCode<3>(ns);
            for (const auto& variable : ns.variables) {
                const std::string variableName = GetFullName(variable);
                stream << Common::newline << Common::tab<3> << fmt::format(R"(.Variable<&{}>("{}"))", variableName, variableName);
                stream << GetMetaDataCode<4>(variable);
            }
            for (const auto& function : ns.functions) {
                const std::string functionName = GetFullName(function);
                stream << Common::newline << Common::tab<3> << fmt::format(R"(.Function<&{}>("{}"))", functionName, functionName);
                stream << GetMetaDataCode<4>(function);
            }
            stream << ";";
        }

        for (const auto& childNs : ns.namespaces) {
            stream << GetNamespaceGlobalScopeCode(childNs, firstBlock);
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
        stream << "/* Generated by mirror tool, do not modify this file anyway. */" << Common::newline;
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
        : metaInfo(inMetaInfo)
        , inputFile(std::move(inInputFile))
        , outputFile(std::move(inOutputFile))
        , headerDirs(std::move(inHeaderDirs))
    {
    }

    Generator::~Generator() = default;

    Generator::Result Generator::Generate() const
    {
        if (const std::filesystem::path parentPath = std::filesystem::path(outputFile).parent_path();
            !std::filesystem::exists(parentPath)) {
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

    Generator::Result Generator::GenerateCode(std::ofstream& file) const
    {
        std::string bestMatchHeaderPath = GetBestMatchHeaderPath(inputFile, headerDirs);
        if (bestMatchHeaderPath.empty()) {
            return std::make_pair(false, "failed to compute best match header path");
        }

        file << GetHeaderNote() << Common::newline;
        file << fmt::format("#include <{}>", bestMatchHeaderPath) << Common::newline;
        file << "#include <Mirror/Registry.h>" << Common::newline;
        file << GetClassesCode(metaInfo);
        return std::make_pair(true, "");
    }
}
