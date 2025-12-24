#include <clang-c/Index.h>

int main(void) {
    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit translationUnit = clang_parseTranslationUnit(index, nullptr, nullptr, 0, nullptr, 0, CXTranslationUnit_None);
    return 0;
}
