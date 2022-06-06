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

    struct PropertyContext {

    };

    struct FunctionContext {

    };

    struct ClassContext {
        std::string name;
        std::string metaData;
        std::vector<FunctionContext> functions;
    };

    struct NamespaceContext {
        std::string name;
        std::vector<ClassContext> classes;
    };

    struct MetaContext {
        std::vector<NamespaceContext> namespaces;
        std::vector<ClassContext> classes;
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
