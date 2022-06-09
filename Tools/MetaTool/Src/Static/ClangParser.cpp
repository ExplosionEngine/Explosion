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
    struct StructClassInnerContext {
        AccessSpecifier currentAccessSpecifier = AccessSpecifier::DEFAULT;
    } structClassInnerContext;
}

namespace MetaTool {
    template <typename T>
    void PopBackIfHasNoMetaData(std::vector<T>& vector)
    {
        if (vector.back().metaData == "") {
            vector.pop_back();
        }
    }

    CXChildVisitResult PFVariable(CXCursor current, CXCursor parent, CXClientData data)
    {
        DebugPrintCursorInfo("PFVariable", current);
        auto* variableContext = static_cast<VariableContext*>(data);

        CXCursorKind kind = clang_getCursorKind(current);
        switch (kind) {
            case CXCursorKind::CXCursor_AnnotateAttr:
            {
                variableContext->metaData = clang_getCString(clang_getCursorSpelling(current));
                break;
            }
            default: break;
        }
        return CXChildVisit_Continue;
    }

    CXChildVisitResult PFFunction(CXCursor current, CXCursor parent, CXClientData data)
    {
        DebugPrintCursorInfo("PFFunction", current);
        auto* functionContext = static_cast<FunctionContext*>(data);

        CXCursorKind kind = clang_getCursorKind(current);
        switch (kind) {
            case CXCursorKind::CXCursor_ParmDecl:
            {
                ParamContext context {};
                context.name = clang_getCString(clang_getCursorSpelling(current));
                context.type = clang_getCString(clang_getTypeSpelling(clang_getCursorType(current)));
                functionContext->params.emplace_back(std::move(context));
                break;
            }
            case CXCursorKind::CXCursor_AnnotateAttr:
            {
                functionContext->metaData = clang_getCString(clang_getCursorSpelling(current));
                break;
            }
            default: break;
        }
        return CXChildVisit_Continue;
    }

    CXChildVisitResult PFStructClass(CXCursor current, CXCursor parent, CXClientData data)
    {
        DebugPrintCursorInfo("PFStructClass", current);
        auto* structContext = static_cast<StructContext*>(data);

        CXCursorKind kind = clang_getCursorKind(current);
        switch (kind) {
            case CXCursorKind::CXCursor_FieldDecl:
            {
                MemberVariableContext context {};
                context.accessSpecifier = structClassInnerContext.currentAccessSpecifier;
                context.name = clang_getCString(clang_getCursorSpelling(current));
                context.type = clang_getCString(clang_getTypeSpelling(clang_getCursorType(current)));
                structContext->variables.emplace_back(std::move(context));
                clang_visitChildren(current, PFVariable, &structContext->variables.back());
                PopBackIfHasNoMetaData(structContext->variables);
                break;
            }
            case CXCursorKind::CXCursor_CXXMethod:
            {
                MemberFunctionContext context {};
                context.accessSpecifier = structClassInnerContext.currentAccessSpecifier;
                context.name = clang_getCString(clang_getCursorSpelling(current));
                context.returnType = clang_getCString(clang_getTypeSpelling(clang_getCursorResultType(current)));
                structContext->functions.emplace_back(std::move(context));
                clang_visitChildren(current, PFFunction, &structContext->functions.back());
                PopBackIfHasNoMetaData(structContext->functions);
                break;
            }
            case CXCursorKind::CXCursor_CXXAccessSpecifier:
            {
                structClassInnerContext.currentAccessSpecifier = static_cast<AccessSpecifier>(clang_getCXXAccessSpecifier(current));
                break;
            }
            case CXCursorKind::CXCursor_AnnotateAttr:
            {
                structContext->metaData = clang_getCString(clang_getCursorSpelling(current));
                break;
            }
            default: break;
        }
        return CXChildVisit_Continue;
    }

    CXChildVisitResult PFNamespace(CXCursor current, CXCursor parent, CXClientData data)
    {
        DebugPrintCursorInfo("PFNamespace", current);
        auto* namespaceContext = static_cast<NamespaceContext*>(data);

        CXCursorKind kind = clang_getCursorKind(current);
        switch (kind) {
            case CXCursorKind::CXCursor_VarDecl:
            {
                VariableContext context {};
                context.name = clang_getCString(clang_getCursorSpelling(current));
                context.type = clang_getCString(clang_getTypeSpelling(clang_getCursorType(current)));
                namespaceContext->variables.emplace_back(std::move(context));
                clang_visitChildren(current, PFVariable, &namespaceContext->variables.back());
                PopBackIfHasNoMetaData(namespaceContext->variables);
                break;
            }
            case CXCursorKind::CXCursor_FunctionDecl:
            {
                FunctionContext context {};
                context.name = clang_getCString(clang_getCursorSpelling(current));
                context.returnType = clang_getCString(clang_getTypeSpelling(clang_getCursorResultType(current)));
                namespaceContext->functions.emplace_back(std::move(context));
                clang_visitChildren(current, PFFunction, &namespaceContext->functions.back());
                PopBackIfHasNoMetaData(namespaceContext->functions);
                break;
            }
            case CXCursorKind::CXCursor_StructDecl:
            {
                StructContext context {};
                context.name = clang_getCString(clang_getCursorSpelling(current));
                namespaceContext->structs.emplace_back(std::move(context));
                structClassInnerContext = StructClassInnerContext();
                clang_visitChildren(current, PFStructClass, &namespaceContext->structs.back());
                PopBackIfHasNoMetaData(namespaceContext->structs);
                break;
            }
            case CXCursorKind::CXCursor_ClassDecl:
            {
                ClassContext context {};
                context.name = clang_getCString(clang_getCursorSpelling(current));
                namespaceContext->classes.emplace_back(std::move(context));
                structClassInnerContext = StructClassInnerContext();
                clang_visitChildren(current, PFStructClass, &namespaceContext->classes.back());
                PopBackIfHasNoMetaData(namespaceContext->classes);
                break;
            }
            case CXCursor_Namespace:
            {
                NamespaceContext context {};
                context.name = clang_getCString(clang_getCursorSpelling(current));
                namespaceContext->namespaces.emplace_back(std::move(context));
                clang_visitChildren(current, PFNamespace, &namespaceContext->namespaces.back());
                break;
            }
            default: break;
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
        metaContext.name = "Global";
        clang_visitChildren(cursor, PFNamespace, &metaContext);
    }

    const MetaContext& ClangParser::GetMetaContext()
    {
        return metaContext;
    }

    std::vector<const char*> ClangParser::GetCommandLineArguments(const SourceInfo& sourceInfo)
    {
        static std::vector<const char*> basicCommandLineArguments = { "-x", "c++" };

        std::vector<const char*> result(basicCommandLineArguments.begin(), basicCommandLineArguments.end());
        result.reserve(result.size() + sourceInfo.includePathNum);
        for (auto i = 0; i < sourceInfo.includePathNum; i++) {
            result.emplace_back("-I");
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
