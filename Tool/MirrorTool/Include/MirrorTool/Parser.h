//
// Created by johnk on 2022/11/21.
//

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <variant>
#include <unordered_map>

#include <clang-c/Index.h>

#include <Common/Utility.h>

namespace MirrorTool {
    enum class FieldAccess {
        pub,
        pro,
        pri,
        max
    };

    using ParamNameAndType = std::pair<std::string, std::string>;
    using MetaDataMap = std::unordered_map<std::string, std::string>;

    struct Node {
        std::string outerName;
        std::string name;
        std::unordered_map<std::string, std::string> metaDatas;
    };

    struct VariableInfo : public Node {
        std::string type;
    };

    struct FunctionInfo : public Node {
        std::string retType;
        std::vector<ParamNameAndType> parameters;
    };

    struct ClassVariableInfo : public VariableInfo {
        FieldAccess fieldAccess;
    };

    struct ClassFunctionInfo : public FunctionInfo {
        FieldAccess fieldAccess;
    };

    struct ClassConstructorInfo : public Node {
        std::vector<ParamNameAndType> parameters;
        FieldAccess fieldAccess;
    };

    struct ClassInfo : public Node {
        FieldAccess lastFieldAccess;
        std::vector<ClassConstructorInfo> constructors;
        std::vector<ClassVariableInfo> staticVariables;
        std::vector<ClassFunctionInfo> staticFunctions;
        std::vector<ClassVariableInfo> variables;
        std::vector<ClassFunctionInfo> functions;
    };

    struct NamespaceInfo : public Node {
        std::vector<VariableInfo> variables;
        std::vector<FunctionInfo> functions;
        std::vector<ClassInfo> classes;
        std::vector<NamespaceInfo> namespaces;
    };

    struct MetaInfo {
        std::vector<NamespaceInfo> namespaces;
        NamespaceInfo global;
    };

    class Parser {
    public:
        using Result = std::pair<bool, std::variant<std::string, MetaInfo>>;

        NonCopyable(Parser)
        explicit Parser(std::string inSourceFile, std::vector<std::string> inHeaderDirs);
        ~Parser();

        Result Parse();

    private:
        static void Cleanup(CXIndex index, CXTranslationUnit translationUnit);
        static Result CleanUpAndConstructFailResult(CXIndex index, CXTranslationUnit translationUnit, std::string reason);

        std::string sourceFile;
        std::vector<std::string> headerDirs;
    };
}
