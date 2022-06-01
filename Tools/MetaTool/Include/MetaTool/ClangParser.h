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

    struct PropertyInfo {

    };

    struct FunctionInfo {

    };

    struct EnumInfo {

    };

    struct ClassInfo {
        std::vector<PropertyInfo> properties;
        std::vector<FunctionInfo> functions;
    };

    struct MetaInfo {
        std::vector<PropertyInfo> globalProperties;
        std::vector<FunctionInfo> globalFunctions;
        std::vector<EnumInfo> enums;
        std::vector<ClassInfo> classes;
    };

    class ClangParser {
    public:
        NON_COPYABLE(ClangParser)
        explicit ClangParser(const SourceInfo& sourceInfo);
        ~ClangParser();

        void Parse();
        const MetaInfo& GetMetaInfo();

    private:
        static std::vector<const char*> GetCommandLineArguments(const SourceInfo& sourceInfo);

        void CreateClangTranslationUnit(const SourceInfo& sourceInfo);
        void DestroyClangTranslationUnit();

        CXIndex clangIndex;
        CXTranslationUnit clangTranslationUnit;
        MetaInfo metaInfo;
    };
}
