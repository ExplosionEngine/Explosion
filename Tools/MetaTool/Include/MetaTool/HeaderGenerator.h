//
// Created by johnk on 2022/5/25.
//

#pragma once

#include <fstream>

#include <Common/Utility.h>

namespace MetaTool {
    struct MetaContext;
    struct NamespaceContext;
    struct ClassContext;
    struct VariableContext;
    struct FunctionContext;

    struct HeaderGeneratorInfo {
        const char* sourceFileShortPath;
        const char* outputFilePath;
    };

    class HeaderGenerator {
    public:
        NON_COPYABLE(HeaderGenerator)
        explicit HeaderGenerator(const HeaderGeneratorInfo& info);
        ~HeaderGenerator();

        void Generate(const MetaTool::MetaContext& metaInfo);

    private:
        void GenerateFileHeader();
        void GenerateIncludes();
        void GenerateRegistry(const MetaTool::MetaContext& metaInfo);
        void GenerateMetaCodeForNamespace(const std::string& prefix, const MetaTool::NamespaceContext& namespaceContext);
        void GenerateMetaCodeForClasses(const std::string& prefix, const MetaTool::ClassContext& classContext);
        void GenerateMetaCodeForProperty(const std::string& prefix, const MetaTool::VariableContext& variableContext);
        void GenerateMetaCodeForFunction(const std::string& prefix, const MetaTool::FunctionContext& functionContext);

        HeaderGeneratorInfo info;
        std::ofstream file;
    };
}
