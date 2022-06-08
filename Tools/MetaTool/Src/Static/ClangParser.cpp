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
    CXChildVisitResult PFFunction(CXCursor current, CXCursor parent, CXClientData data)
    {
        DebugPrintCursorInfo("PFFunction", current);
        auto* functionContext = static_cast<FunctionContext*>(data);

        CXCursorKind kind = clang_getCursorKind(current);
        switch (kind) {
            case CXCursorKind::CXCursor_ParmDecl:
            {
                functionContext->paramNames.emplace_back(clang_getCString(clang_getCursorSpelling(current)));
                functionContext->paramTypes.emplace_back(clang_getCString(clang_getTypeSpelling(clang_getCursorType(current))));
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
                MemberVariableContext memberVariableContext {};
                memberVariableContext.accessSpecifier = structClassInnerContext.currentAccessSpecifier;
                memberVariableContext.name = clang_getCString(clang_getCursorSpelling(current));
                memberVariableContext.type = clang_getCString(clang_getTypeSpelling(clang_getCursorType(current)));
                structContext->variables.emplace_back(std::move(memberVariableContext));
                break;
            }
            case CXCursorKind::CXCursor_CXXMethod:
            {
                MemberFunctionContext memberFunctionContext {};
                memberFunctionContext.accessSpecifier = structClassInnerContext.currentAccessSpecifier;
                memberFunctionContext.name = clang_getCString(clang_getCursorSpelling(current));
                memberFunctionContext.prototype = clang_getCString(clang_getTypeSpelling(clang_getCursorType(current)));
                memberFunctionContext.returnType = clang_getCString(clang_getTypeSpelling(clang_getCursorResultType(current)));
                structContext->functions.emplace_back(std::move(memberFunctionContext));
                clang_visitChildren(current, PFFunction, &structContext->functions.back());
                break;
            }
            case CXCursorKind::CXCursor_CXXAccessSpecifier:
            {
                structClassInnerContext.currentAccessSpecifier = static_cast<AccessSpecifier>(clang_getCXXAccessSpecifier(current));
                break;
            }
            default: break;
        }
        return CXChildVisit_Continue;
    }

    void ParseScopeChildCursor(ScopeContext* scopeContext, CXCursor current, CXCursorKind kind)
    {
        switch (kind) {
            case CXCursorKind::CXCursor_VarDecl:
            {
                VariableContext variableContext {};
                variableContext.name = clang_getCString(clang_getCursorSpelling(current));
                variableContext.type = clang_getCString(clang_getTypeSpelling(clang_getCursorType(current)));
                scopeContext->variables.emplace_back(std::move(variableContext));
                break;
            }
            case CXCursorKind::CXCursor_FunctionDecl:
            {
                FunctionContext functionContext {};
                functionContext.name = clang_getCString(clang_getCursorSpelling(current));
                functionContext.prototype = clang_getCString(clang_getTypeSpelling(clang_getCursorType(current)));
                functionContext.returnType = clang_getCString(clang_getTypeSpelling(clang_getCursorResultType(current)));
                scopeContext->functions.emplace_back(std::move(functionContext));
                clang_visitChildren(current, PFFunction, &scopeContext->functions.back());
                break;
            }
            case CXCursorKind::CXCursor_StructDecl:
            {
                StructContext structContext {};
                structContext.name = clang_getCString(clang_getCursorSpelling(current));
                scopeContext->structs.emplace_back(std::move(structContext));
                structClassInnerContext = StructClassInnerContext();
                clang_visitChildren(current, PFStructClass, &scopeContext->structs.back());
                break;
            }
            case CXCursorKind::CXCursor_ClassDecl:
            {
                ClassContext classContext {};
                classContext.name = clang_getCString(clang_getCursorSpelling(current));
                scopeContext->classes.emplace_back(std::move(classContext));
                structClassInnerContext = StructClassInnerContext();
                clang_visitChildren(current, PFStructClass, &scopeContext->classes.back());
                break;
            }
            default: break;
        }
    }

    CXChildVisitResult PFNamespace(CXCursor current, CXCursor parent, CXClientData data)
    {
        DebugPrintCursorInfo("PFNamespace", current);
        auto* namespaceContext = static_cast<NamespaceContext*>(data);

        CXCursorKind kind = clang_getCursorKind(current);
        ParseScopeChildCursor(namespaceContext, current, kind);
        return CXChildVisit_Continue;
    }

    CXChildVisitResult PFMain(CXCursor current, CXCursor parent, CXClientData data)
    {
        DebugPrintCursorInfo("PFMain", current);
        auto* metaContext = static_cast<MetaContext*>(data);

        CXCursorKind kind = clang_getCursorKind(current);
        switch (kind) {
            case CXCursor_Namespace:
            {
                NamespaceContext namespaceContext {};
                namespaceContext.name = clang_getCString(clang_getCursorSpelling(current));
                metaContext->namespaces.emplace_back(std::move(namespaceContext));
                clang_visitChildren(current, PFNamespace, &metaContext->namespaces.back());
                break;
            }
            default:
            {
                ParseScopeChildCursor(metaContext, current, kind);
                break;
            }
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
        clang_visitChildren(cursor, PFMain, &metaContext);
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
