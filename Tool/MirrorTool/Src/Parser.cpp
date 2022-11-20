//
// Created by johnk on 2022/11/21.
//

#include <MirrorTool/Parser.h>

namespace MirrorTool {
    Parser::Parser(std::string inSourceFile) : sourceFile(std::move(inSourceFile)) {}

    Parser::~Parser() = default;

    Parser::Result Parser::Parse()
    {
        CXIndex index = clang_createIndex(0, 0);
        CXTranslationUnit translationUnit = clang_parseTranslationUnit(index, sourceFile.c_str(), nullptr, 0, nullptr, 0, CXTranslationUnit_None);
        if (translationUnit == nullptr) {
            return CleanUpAndConstructFailResult(index, translationUnit, "failed to create translation unit from source file");
        }

        // TODO

        Cleanup(index, translationUnit);
        // TODO
        return {};
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
