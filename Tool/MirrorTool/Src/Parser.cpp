//
// Created by johnk on 2022/11/21.
//

#include <functional>

#include <MirrorTool/Parser.h>

#define VisitChildren(funcName, contextType, cursor, context) \
    clang_visitChildren(cursor, [](CXCursor c, CXCursor p, CXClientData cd) -> CXChildVisitResult { \
        auto* ctx = reinterpret_cast<contextType*>(cd); \
        return MirrorTool::funcName(c, p, *ctx); \
    }, &context); \

namespace MirrorTool {
    static CXChildVisitResult OutermostVisitor(CXCursor cursor, CXCursor parent, MetaInfo& context)
    {
        // TODO
        return CXChildVisit_Continue;
    }
}

namespace MirrorTool {
    Parser::Parser(std::string inSourceFile, std::vector<std::string> inHeaderDirs) : sourceFile(std::move(inSourceFile)), headerDirs(std::move(inHeaderDirs)) {}

    Parser::~Parser() = default;

    Parser::Result Parser::Parse()
    {
        std::vector<std::string> argumentStrs;
        argumentStrs.reserve(headerDirs.size());
        for (const std::string& headerDir : headerDirs) {
            argumentStrs.emplace_back(std::string("-I") + headerDir);
        }

        std::vector<const char*> arguments(argumentStrs.size());
        for (auto i = 0; i < arguments.size(); i++) {
            arguments.emplace_back(argumentStrs[i].c_str());
        }

        CXIndex index = clang_createIndex(0, 0);
        CXTranslationUnit translationUnit = clang_parseTranslationUnit(index, sourceFile.c_str(), arguments.data(), static_cast<int>(arguments.size()), nullptr, 0, CXTranslationUnit_None);
        if (translationUnit == nullptr) {
            return CleanUpAndConstructFailResult(index, translationUnit, "failed to create translation unit from source file");
        }

        MetaInfo metaInfo;
        CXCursor cursor = clang_getTranslationUnitCursor(translationUnit);

        VisitChildren(OutermostVisitor, MetaInfo, cursor, metaInfo);

        Cleanup(index, translationUnit);
        return std::make_pair(true, std::move(metaInfo));
    }

    void Parser::Cleanup(CXIndex index, CXTranslationUnit translationUnit)
    {
        if (translationUnit != nullptr) {
            clang_disposeTranslationUnit(translationUnit);
        }
        if (index != nullptr) {
            clang_disposeIndex(index);
        }
    }

    Parser::Result Parser::CleanUpAndConstructFailResult(CXIndex index, CXTranslationUnit translationUnit, std::string reason)
    {
        Cleanup(index, translationUnit);
        return std::make_pair(false, std::move(reason));
    }
}
