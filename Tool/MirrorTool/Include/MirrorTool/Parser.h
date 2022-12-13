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
    struct Node {
        std::string name;
        std::unordered_map<std::string, std::string> metaDatas;
    };

    struct VariableInfo : public Node {
        std::string type;
    };

    struct FunctionInfo : public Node {
        std::string retType;
        std::vector<std::string> argTypes;
    };

    struct ClassInfo : public Node {
        std::vector<VariableInfo> staticVariables;
        std::vector<FunctionInfo> staticFunctions;
        std::vector<VariableInfo> variables;
        std::vector<FunctionInfo> functions;
    };

    struct NamespaceInfo {
        std::vector<VariableInfo> variables;
        std::vector<FunctionInfo> functions;
        std::vector<ClassInfo> classes;
    };

    struct MetaInfo {
        std::vector<NamespaceInfo> namespaces;
        NamespaceInfo global;
    };

    class Parser {
    public:
        using Result = std::pair<bool, std::variant<std::string, MetaInfo>>;

        NON_COPYABLE(Parser)
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
