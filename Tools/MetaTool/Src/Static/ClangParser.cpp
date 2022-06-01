//
// Created by johnk on 2022/5/25.
//

#include <iostream>
#include <functional>

#include <MetaTool/ClangParser.h>
#include <Common/Debug.h>

namespace MetaTool {
#define DEBUG_OUTPUT 1

    void DebugPrintCursorInfo(const std::string& tag, CXCursor cursor)
    {
#if BUILD_CONFIG_DEBUG && DEBUG_OUTPUT
        CXCursorKind kind = clang_getCursorKind(cursor);
        std::cout << tag << ": " << clang_getCString(clang_getCursorSpelling(cursor)) << ", " << clang_getCString(clang_getCursorKindSpelling(kind)) << std::endl;
#endif
    }
}

namespace MetaTool {
    struct NamespaceContext {
        std::string name;
    };

    struct StructContext {
        NamespaceContext* nameSpaceContext;
        std::string name;
    };
}

namespace MetaTool {
    CXChildVisitResult PFStruct(CXCursor current, CXCursor parent, CXClientData data)
    {
        DebugPrintCursorInfo("PFStruct", current);
        auto* structContext = static_cast<StructContext*>(data);

        // TODO
        return CXChildVisit_Continue;
    }

    CXChildVisitResult PFNameSpace(CXCursor current, CXCursor parent, CXClientData data)
    {
        DebugPrintCursorInfo("PFNameSpace", current);
        auto* nameSpaceContext = static_cast<NamespaceContext*>(data);

        CXCursorKind kind = clang_getCursorKind(current);
        switch (kind) {
            case CXCursorKind::CXCursor_StructDecl:
            {
                StructContext structContext {};
                structContext.nameSpaceContext = nameSpaceContext;
                clang_visitChildren(current, PFStruct, &structContext);
            }
            default:
                break;
        }
        return CXChildVisit_Continue;
    }

    CXChildVisitResult PFMain(CXCursor current, CXCursor parent, CXClientData data)
    {
        DebugPrintCursorInfo("PFMain", current);
        auto* metaInfo = static_cast<MetaInfo*>(data);

        CXCursorKind kind = clang_getCursorKind(current);
        switch (kind) {
            case CXCursorKind::CXCursor_StructDecl:
                {
                    StructContext structContext {};
                    structContext.name = clang_getCString(clang_getCursorSpelling(current));
                    clang_visitChildren(current, PFStruct, &structContext);
                }
                break;
            case CXCursorKind::CXCursor_Namespace:
                {
                    NamespaceContext namespaceContext {};
                    namespaceContext.name = clang_getCString(clang_getCursorSpelling(current));
                    clang_visitChildren(current, PFNameSpace, &namespaceContext);
                }
            default:
                break;
        }
        return CXChildVisit_Continue;
    }
}

namespace MetaTool {
    ClangParser::ClangParser(const SourceInfo& sourceInfo) : clangIndex(nullptr), clangTranslationUnit(nullptr)
    {
        CreateClangTranslationUnit(sourceInfo);
    }

    ClangParser::~ClangParser()
    {
        DestroyClangTranslationUnit();
    }

    void ClangParser::Parse()
    {
        CXCursor cursor = clang_getTranslationUnitCursor(clangTranslationUnit);
        clang_visitChildren(cursor, PFMain, &metaInfo);
    }

    const MetaInfo& ClangParser::GetMetaInfo()
    {
        return metaInfo;
    }

    std::vector<const char*> ClangParser::GetCommandLineArguments(const SourceInfo& sourceInfo)
    {
        static std::vector<const char*> basicCommandLineArguments = { "-x", "c++" };

        std::vector<const char*> result(basicCommandLineArguments.begin(), basicCommandLineArguments.end());
        result.reserve(result.size() + sourceInfo.includePathNum);
        for (auto i = 0; i < sourceInfo.includePathNum; i++) {
            result.emplace_back(sourceInfo.includePaths[i]);
        }
        return result;
    }

    void ClangParser::CreateClangTranslationUnit(const SourceInfo& sourceInfo)
    {
        auto cmdArguments = GetCommandLineArguments(sourceInfo);

        clangIndex = clang_createIndex(0, 0);
        Assert(clangIndex);

        clangTranslationUnit = clang_parseTranslationUnit(
            clangIndex, sourceInfo.sourceFile,
            cmdArguments.data(), static_cast<int32_t>(cmdArguments.size()),
            nullptr, 0, CXTranslationUnit_None);
        Assert(clangIndex);
    }

    void ClangParser::DestroyClangTranslationUnit()
    {
        clang_disposeTranslationUnit(clangTranslationUnit);
        clang_disposeIndex(clangIndex);
    }
}
