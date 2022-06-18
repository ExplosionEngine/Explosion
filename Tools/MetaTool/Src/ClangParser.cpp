//
// Created by johnk on 2022/5/25.
//

#include <iostream>
#include <functional>

#include <MetaTool/ClangParser.h>
#include <Common/Debug.h>

namespace MetaTool {
#define DEBUG_OUTPUT 0

    void DebugPrintCursorInfo(const std::string& tag, CXCursor cursor)
    {
#if BUILD_CONFIG_DEBUG && DEBUG_OUTPUT
        CXCursorKind kind = clang_getCursorKind(cursor);
        std::cout << tag << ": " << clang_getCString(clang_getCursorSpelling(cursor)) << ", " << clang_getCString(clang_getCursorKindSpelling(kind)) << std::endl;
#endif
    }
}

namespace MetaTool {
    struct ClassInnerContext {
        AccessSpecifier currentAccessSpecifier = AccessSpecifier::DEFAULT;
    } classInnerContext;
}

namespace MetaTool {
    AccessSpecifier GetActualAccessSpecifier(CXCursorKind parrentKind, AccessSpecifier accessSpecifier)
    {
        if (parrentKind == CXCursor_ClassDecl) {
            return accessSpecifier == AccessSpecifier::DEFAULT ? AccessSpecifier::PRIVATE : accessSpecifier;
        } else if (parrentKind == CXCursor_StructDecl) {
            return accessSpecifier == AccessSpecifier::DEFAULT ? AccessSpecifier::PUBLIC : accessSpecifier;
        } else {
            return AccessSpecifier::DEFAULT;
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

    CXChildVisitResult PFClass(CXCursor current, CXCursor parent, CXClientData data)
    {
        DebugPrintCursorInfo("PFClass", current);
        auto* classContext = static_cast<ClassContext*>(data);

        CXCursorKind kind = clang_getCursorKind(current);
        switch (kind) {
            case CXCursorKind::CXCursor_FieldDecl:
            {
                VariableContext context {};
                AccessSpecifier accessSpecifier = GetActualAccessSpecifier(clang_getCursorKind(parent), classInnerContext.currentAccessSpecifier);
                context.name = clang_getCString(clang_getCursorSpelling(current));
                context.type = clang_getCString(clang_getTypeSpelling(clang_getCursorType(current)));
                classContext->variables.emplace_back(std::move(context));
                clang_visitChildren(current, PFVariable, &classContext->variables.back());
                if (classContext->variables.back().metaData.empty() || accessSpecifier != AccessSpecifier::PUBLIC) {
                    classContext->variables.pop_back();
                }
                break;
            }
            case CXCursorKind::CXCursor_CXXMethod:
            {
                FunctionContext context {};
                AccessSpecifier accessSpecifier = GetActualAccessSpecifier(clang_getCursorKind(parent), classInnerContext.currentAccessSpecifier);
                context.name = clang_getCString(clang_getCursorSpelling(current));
                context.returnType = clang_getCString(clang_getTypeSpelling(clang_getCursorResultType(current)));
                classContext->functions.emplace_back(std::move(context));
                clang_visitChildren(current, PFFunction, &classContext->functions.back());
                if (classContext->functions.back().metaData.empty() || accessSpecifier != AccessSpecifier::PUBLIC) {
                    classContext->functions.pop_back();
                }
                break;
            }
            case CXCursorKind::CXCursor_Constructor:
            {
                FunctionContext context {};
                AccessSpecifier accessSpecifier = GetActualAccessSpecifier(clang_getCursorKind(parent), classInnerContext.currentAccessSpecifier);
                classContext->constructors.emplace_back(std::move(context));
                clang_visitChildren(current, PFFunction, &classContext->constructors.back());
                if (classContext->constructors.back().metaData.empty() || accessSpecifier != AccessSpecifier::PUBLIC) {
                    classContext->constructors.pop_back();
                }
                break;
            }
            case CXCursorKind::CXCursor_CXXAccessSpecifier:
            {
                classInnerContext.currentAccessSpecifier = static_cast<AccessSpecifier>(clang_getCXXAccessSpecifier(current));
                break;
            }
            case CXCursorKind::CXCursor_AnnotateAttr:
            {
                classContext->metaData = clang_getCString(clang_getCursorSpelling(current));
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
            case CXCursorKind::CXCursor_StructDecl:
            {
                ClassContext context {};
                context.name = clang_getCString(clang_getCursorSpelling(current));
                namespaceContext->classes.emplace_back(std::move(context));
                classInnerContext = ClassInnerContext();
                clang_visitChildren(current, PFClass, &namespaceContext->classes.back());
                if (namespaceContext->classes.back().metaData.empty()) {
                    namespaceContext->classes.pop_back();
                }
                break;
            }
            case CXCursorKind::CXCursor_ClassDecl:
            {
                ClassContext context {};
                context.name = clang_getCString(clang_getCursorSpelling(current));
                namespaceContext->classes.emplace_back(std::move(context));
                classInnerContext = ClassInnerContext();
                clang_visitChildren(current, PFClass, &namespaceContext->classes.back());
                if (namespaceContext->classes.back().metaData.empty()) {
                    namespaceContext->classes.pop_back();
                }
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
        clang_visitChildren(cursor, PFNamespace, &metaContext);
    }

    const MetaContext& ClangParser::GetMetaContext()
    {
        return metaContext;
    }

    std::vector<const char*> ClangParser::GetCommandLineArguments(const SourceInfo& sourceInfo)
    {
        static std::vector<const char*> basicCommandLineArguments = { "-x", "c++", "-DMETA_TOOL" };

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
