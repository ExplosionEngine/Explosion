//
// Created by johnk on 2022/5/25.
//

#pragma once

#include <cstdint>
#include <vector>

#include <clang-c/Index.h>

#include <Common/Utility.h>

namespace MetaTool {
    struct SourceInfo {
        const char* sourceFile;
        uint32_t includePathNum;
        const char** includePaths;
    };

    enum class AccessSpecifier {
        DEFAULT,
        PUBLIC,
        PROTECTED,
        PRIVATE,
        MAX
    };

    struct VariableContext {
        std::string name;
        std::string type;
    };

    struct FunctionContext {
        std::string name;
        std::string prototype;
        std::string returnType;
        std::vector<std::string> paramNames;
        std::vector<std::string> paramTypes;
    };

    struct MemberVariableContext : VariableContext {
        AccessSpecifier accessSpecifier = AccessSpecifier::DEFAULT;
    };

    struct MemberFunctionContext : FunctionContext {
        AccessSpecifier accessSpecifier = AccessSpecifier::DEFAULT;
    };

    struct StructContext {
        std::string name;
        std::vector<MemberVariableContext> variables;
        std::vector<MemberFunctionContext> functions;
    };

    struct ClassContext : public StructContext {};

    struct ScopeContext {
        std::vector<VariableContext> variables;
        std::vector<FunctionContext> functions;
        std::vector<StructContext> structs;
        std::vector<ClassContext> classes;
    };

    struct NamespaceContext : public ScopeContext {
        std::string name;
    };

    struct MetaContext : public ScopeContext {
        std::vector<NamespaceContext> namespaces;
    };

    class ClangParser {
    public:
        NON_COPYABLE(ClangParser)
        explicit ClangParser(const SourceInfo& sourceInfo);
        ~ClangParser();

        void Parse();
        const MetaContext& GetMetaContext();

    private:
        static std::vector<const char*> GetCommandLineArguments(const SourceInfo& sourceInfo);

        void CreateClangTranslationUnit(const SourceInfo& sourceInfo);
        void DestroyClangTranslationUnit();

        CXIndex clangIndex;
        CXTranslationUnit clangTranslationUnit;
        MetaContext metaContext;
    };
}
