//
// Created by johnk on 2022/11/21.
//

#include <sstream>

#include <Common/String.h>
#include <MirrorTool/Parser.h>

#define DEBUG_OUTPUT 0

#if DEBUG_OUTPUT
#include <iostream>
#endif

#define DeclareVisitor(visitorName, contextType) \
    static CXChildVisitResult visitorName(CXCursor cursor, CXCursor parent, contextType& context) \

#define VisitChildren(funcName, contextType, cursor, context) \
    clang_visitChildren(cursor, [](CXCursor c, CXCursor p, CXClientData cd) -> CXChildVisitResult { \
        auto* ctx = reinterpret_cast<contextType*>(cd); \
        return MirrorTool::funcName(c, p, *ctx); \
    }, &context); \

#define FetchCursorInfo(visitorName, cursorVar) \
    PrintDebugInfo(#visitorName, cursorVar); \
    CXCursorKind kind = clang_getCursorKind(cursorVar); \
    CXString spelling = clang_getCursorSpelling(cursorVar); \
    std::string spellingStr = clang_getCString(spelling); \

#define CleanUpAndContinueVisit \
    clang_disposeString(spelling); \
    return CXChildVisit_Continue;  \

#define ProcessVariable \
    if (kind == CXCursor_AnnotateAttr) { \
        ParseMetaDatas(context, spellingStr); \
    } \

#define ProcessFunction \
    if (kind == CXCursor_AnnotateAttr) { \
        ParseMetaDatas(context, spellingStr); \
    } else if (kind == CXCursor_ParmDecl) { \
        CXType type = clang_getCursorType(cursor); \
        CXString typeSpelling = clang_getTypeSpelling(type); \
        \
        ParamNameAndType paramInfo; \
        paramInfo.first = spellingStr; \
        paramInfo.second = clang_getCString(typeSpelling); \
        context.parameters.emplace_back(std::move(paramInfo)); \
        \
        clang_disposeString(typeSpelling); \
    } \

static void PrintDebugInfo(const std::string& visitorName, CXCursor cursor)
{
#if DEBUG_OUTPUT
    CXCursorKind kind = clang_getCursorKind(cursor);
    CXString kindSpelling = clang_getCursorKindSpelling(kind);
    CXString spelling = clang_getCursorSpelling(cursor);
    std::cout << visitorName << ": " << clang_getCString(kindSpelling) << ", " << clang_getCString(spelling) << std::endl;
    clang_disposeString(kindSpelling);
    clang_disposeString(spelling);
#endif
}

namespace MirrorTool {
    static constexpr auto propertyMetaTag = "property";
    static constexpr auto functionMetaTag = "func";
    static constexpr auto classMetaTag = "class";
    static constexpr auto enumMetaTag = "enum";

    static FieldAccess GetFieldAccess(CX_CXXAccessSpecifier accessSpecifier)
    {
        static std::unordered_map<CX_CXXAccessSpecifier, FieldAccess> map = {
            { CX_CXXPublic, FieldAccess::pub },
            { CX_CXXProtected, FieldAccess::pro },
            { CX_CXXPrivate, FieldAccess::pri }
        };
        return map.at(accessSpecifier);
    }

    static std::string GetPureBaseClassName(const std::string& str)
    {
        const auto result = Common::StringUtils::Replace(str, "class ", "");
        return Common::StringUtils::Replace(result, "struct", "");
    }

    static std::string RemoveStrSpace(const std::string& value)
    {
        return Common::StringUtils::Replace(value, " ", "");
    }

    static void ParseMetaDatas(Node& node, const std::string& metaDataStr)
    {
        for (const auto metaDatas = Common::StringUtils::Split(metaDataStr, ",");
            const auto& metaData : metaDatas) {
            if (auto keyValue = Common::StringUtils::Split(metaData, "=");
                keyValue.size() == 1) {
                node.metaDatas.emplace(std::make_pair(RemoveStrSpace(keyValue[0]), "true"));
            } else if (keyValue.size() == 2) {
                node.metaDatas.emplace(std::make_pair(RemoveStrSpace(keyValue[0]), RemoveStrSpace(keyValue[1])));
            }
        }
    }

    static std::string GetOuterName(const std::string& basic, const std::string& name)
    {
        return basic.empty() ? name : (basic + "::" + name);
    }

    template <typename T>
    static bool PopoutIfHaveNoMetaTag(std::vector<T>& container, const std::string& tag)
    {
        const MetaDataMap& metaDatas = container.back().metaDatas;

        bool found = false;
        for (const auto& [key, value] : metaDatas) {
            if (key == tag && value == "true") {
                found = true;
                break;
            }
        }
        if (!found) {
            container.pop_back();
        }
        return found;
    }

    template <typename T>
    static void ClearMetaTag(T& node, const std::string& tag)
    {
        MetaDataMap& metaDatas = node.metaDatas;
        const auto iter = metaDatas.find(tag);
        if (iter == metaDatas.end()) {
            return;
        }
        metaDatas.erase(iter);
    }

    template <typename T>
    static void ApplyMetaFilter(std::vector<T>& container, const std::string& tag)
    {
        if (PopoutIfHaveNoMetaTag(container, tag)) {
            ClearMetaTag(container.back(), tag);
        }
    }

    static void UpdateConstructorName(ClassConstructorInfo& info)
    {
        std::stringstream stream;
        for (auto i = 0; i < info.parameters.size(); i++) {
            stream << info.parameters[i].second;
            if (i != info.parameters.size() - 1) {
                stream << ", ";
            }
        }
        info.name = stream.str();
    }

    DeclareVisitor(GlobalVariableVisitor, VariableInfo)
    {
        FetchCursorInfo(GlobalVariableVisitor, cursor);
        ProcessVariable;
        CleanUpAndContinueVisit;
    }

    DeclareVisitor(GlobalFunctionVisitor, FunctionInfo)
    {
        FetchCursorInfo(GlobalFunctionVisitor, cursor);
        ProcessFunction;
        CleanUpAndContinueVisit;
    }

    DeclareVisitor(ClassVariableVisitor, ClassVariableInfo)
    {
        FetchCursorInfo(ClassVariableVisitor, cursor);
        ProcessVariable;
        CleanUpAndContinueVisit;
    }

    DeclareVisitor(ClassFunctionVisitor, ClassFunctionInfo)
    {
        FetchCursorInfo(ClassFunctionVisitor, cursor);
        ProcessFunction;
        CleanUpAndContinueVisit;
    }

    DeclareVisitor(ClassConstructorVisitor, ClassConstructorInfo)
    {
        FetchCursorInfo(ClassConstructorVisitor, cursor);
        ProcessFunction;
        CleanUpAndContinueVisit;
    }

    DeclareVisitor(ClassVisitor, ClassInfo)
    {
        FetchCursorInfo(ClassVisitor, cursor);
        if (kind == CXCursor_AnnotateAttr) {
            ParseMetaDatas(context, spellingStr);
        } else if (kind == CXCursor_CXXAccessSpecifier) {
            context.lastFieldAccess = GetFieldAccess(clang_getCXXAccessSpecifier(cursor));
        } else if (kind == CXCursor_CXXBaseSpecifier) {
            context.baseClassName = GetPureBaseClassName(spellingStr);
        } else if (kind == CXCursor_VarDecl || kind == CXCursor_FieldDecl) {
            CXType type = clang_getCursorType(cursor);
            CXString typeSpelling = clang_getTypeSpelling(type);

            ClassVariableInfo variableInfo;
            variableInfo.outerName = GetOuterName(context.outerName, context.name);
            variableInfo.name = spellingStr;
            variableInfo.type = clang_getCString(typeSpelling);
            variableInfo.fieldAccess = context.lastFieldAccess;
            auto& variables = kind == CXCursor_VarDecl ? context.staticVariables : context.variables;
            variables.emplace_back(std::move(variableInfo));
            VisitChildren(ClassVariableVisitor, ClassVariableInfo, cursor, variables.back());
            ApplyMetaFilter(variables, propertyMetaTag);

            clang_disposeString(typeSpelling);
        } else if (kind == CXCursor_CXXMethod) {
            bool isStatic = static_cast<bool>(clang_CXXMethod_isStatic(cursor));
            CXType retType = clang_getCursorResultType(cursor);
            CXString retTypeSpelling = clang_getTypeSpelling(retType);

            ClassFunctionInfo functionInfo;
            functionInfo.outerName = GetOuterName(context.outerName, context.name);
            functionInfo.name = spellingStr;
            functionInfo.retType = clang_getCString(retTypeSpelling);
            functionInfo.fieldAccess = context.lastFieldAccess;
            auto& functions = isStatic ? context.staticFunctions : context.functions;
            functions.emplace_back(std::move(functionInfo));
            VisitChildren(ClassFunctionVisitor, ClassFunctionInfo, cursor, functions.back());
            ApplyMetaFilter(functions, functionMetaTag);

            clang_disposeString(retTypeSpelling);
        } else if (kind == CXCursor_Constructor) {
            ClassConstructorInfo constructorInfo;
            constructorInfo.outerName = GetOuterName(context.outerName, context.name);
            constructorInfo.fieldAccess = context.lastFieldAccess;
            context.constructors.emplace_back(std::move(constructorInfo));
            VisitChildren(ClassConstructorVisitor, ClassConstructorInfo, cursor, context.constructors.back());
            UpdateConstructorName(context.constructors.back());
        } else if (kind == CXCursor_Destructor) {
            ClassDestructorInfo destructorInfo;
            destructorInfo.outerName = GetOuterName(context.outerName, context.name);
            destructorInfo.fieldAccess = context.lastFieldAccess;
            context.destructor = std::move(destructorInfo);
        } else if (kind == CXCursor_StructDecl || kind == CXCursor_ClassDecl) {
            ClassInfo classInfo;
            classInfo.outerName = GetOuterName(context.outerName, context.name);
            classInfo.name = spellingStr;
            classInfo.lastFieldAccess = kind == CXCursor_StructDecl ? FieldAccess::pub : FieldAccess::pri;
            context.classes.emplace_back(std::move(classInfo));
            VisitChildren(ClassVisitor, ClassInfo, cursor, context.classes.back());
            ApplyMetaFilter(context.classes, classMetaTag);
        }
        CleanUpAndContinueVisit;
    }

    DeclareVisitor(EnumElementVisitor, EnumElementInfo)
    {
        FetchCursorInfo(EnumElementVisitor, cursor);
        if (kind == CXCursor_AnnotateAttr) {
            ParseMetaDatas(context, spellingStr);
        }
        CleanUpAndContinueVisit;
    }

    DeclareVisitor(EnumVisitor, EnumInfo)
    {
        FetchCursorInfo(EnumVisitor, cursor);
        if (kind == CXCursor_AnnotateAttr) {
            ParseMetaDatas(context, spellingStr);
        } else if (kind == CXCursor_EnumConstantDecl) {
            EnumElementInfo elementInfo;
            elementInfo.outerName = GetOuterName(context.outerName, context.name);
            elementInfo.name = spellingStr;
            context.elements.emplace_back(std::move(elementInfo));
            VisitChildren(EnumElementVisitor, EnumElementInfo, cursor, context.elements.back());
        }
        CleanUpAndContinueVisit;
    }

    DeclareVisitor(NamespaceVisitor, NamespaceInfo)
    {
        FetchCursorInfo(NamespaceVisitor, cursor);
        if (kind == CXCursor_AnnotateAttr) {
            ParseMetaDatas(context, spellingStr);
        } else if (kind == CXCursor_VarDecl) {
            CXType type = clang_getCursorType(cursor);
            CXString typeSpelling = clang_getTypeSpelling(type);

            VariableInfo variableInfo;
            variableInfo.outerName = GetOuterName(context.outerName, context.name);
            variableInfo.name = spellingStr;
            variableInfo.type = clang_getCString(typeSpelling);
            context.variables.emplace_back(std::move(variableInfo));
            VisitChildren(GlobalVariableVisitor, VariableInfo, cursor, context.variables.back());
            ApplyMetaFilter(context.variables, propertyMetaTag);

            clang_disposeString(typeSpelling);
        } else if (kind == CXCursor_FunctionDecl) {
            CXType retType = clang_getCursorResultType(cursor);
            CXString retTypeSpelling = clang_getTypeSpelling(retType);

            FunctionInfo functionInfo;
            functionInfo.outerName = GetOuterName(context.outerName, context.name);
            functionInfo.name = spellingStr;
            functionInfo.retType = clang_getCString(retTypeSpelling);
            context.functions.emplace_back(std::move(functionInfo));
            VisitChildren(GlobalFunctionVisitor, FunctionInfo, cursor, context.functions.back());
            ApplyMetaFilter(context.functions, functionMetaTag);

            clang_disposeString(retTypeSpelling);
        } else if (kind == CXCursor_StructDecl || kind == CXCursor_ClassDecl) {
            ClassInfo classInfo;
            classInfo.outerName = GetOuterName(context.outerName, context.name);
            classInfo.name = spellingStr;
            classInfo.lastFieldAccess = kind == CXCursor_StructDecl ? FieldAccess::pub : FieldAccess::pri;
            context.classes.emplace_back(std::move(classInfo));
            VisitChildren(ClassVisitor, ClassInfo, cursor, context.classes.back());
            ApplyMetaFilter(context.classes, classMetaTag);
        } else if (kind == CXCursor_EnumDecl) {
            EnumInfo enumInfo;
            enumInfo.outerName = GetOuterName(context.outerName, context.name);
            enumInfo.name = spellingStr;
            context.enums.emplace_back(std::move(enumInfo));
            VisitChildren(EnumVisitor, EnumInfo, cursor, context.enums.back());
            ApplyMetaFilter(context.enums, enumMetaTag);
        } else if (kind == CXCursor_Namespace) {
            NamespaceInfo namespaceInfo;
            namespaceInfo.outerName = GetOuterName(context.outerName, context.name);
            namespaceInfo.name = spellingStr;
            context.namespaces.emplace_back(std::move(namespaceInfo));
            VisitChildren(NamespaceVisitor, NamespaceInfo, cursor, context.namespaces.back());
        }
        CleanUpAndContinueVisit;
    }

    DeclareVisitor(OutermostVisitor, MetaInfo)
    {
        FetchCursorInfo(OutermostVisitor, cursor);
        CXSourceLocation sourceLocation = clang_getCursorLocation(cursor);
        if (clang_Location_isFromMainFile(sourceLocation) == 0) {
            return CXChildVisit_Continue;
        }

        if (kind == CXCursor_VarDecl) {
            CXType type = clang_getCursorType(cursor);
            CXString typeSpelling = clang_getTypeSpelling(type);

            VariableInfo variableInfo;
            variableInfo.name = spellingStr;
            variableInfo.type = clang_getCString(typeSpelling);
            context.global.variables.emplace_back(std::move(variableInfo));
            VisitChildren(GlobalVariableVisitor, VariableInfo, cursor, context.global.variables.back());
            ApplyMetaFilter(context.global.variables, propertyMetaTag);

            clang_disposeString(typeSpelling);
        } else if (kind == CXCursor_FunctionDecl) {
            CXType retType = clang_getCursorResultType(cursor);
            CXString retTypeSpelling = clang_getTypeSpelling(retType);

            FunctionInfo functionInfo;
            functionInfo.name = spellingStr;
            functionInfo.retType = clang_getCString(retTypeSpelling);
            context.global.functions.emplace_back(std::move(functionInfo));
            VisitChildren(GlobalFunctionVisitor, FunctionInfo, cursor, context.global.functions.back());
            ApplyMetaFilter(context.global.functions, functionMetaTag);

            clang_disposeString(retTypeSpelling);
        } else if (kind == CXCursor_StructDecl || kind == CXCursor_ClassDecl) {
            ClassInfo classInfo;
            classInfo.name = spellingStr;
            classInfo.lastFieldAccess = kind == CXCursor_StructDecl ? FieldAccess::pub : FieldAccess::pri;
            context.global.classes.emplace_back(std::move(classInfo));
            VisitChildren(ClassVisitor, ClassInfo, cursor, context.global.classes.back());
            ApplyMetaFilter(context.global.classes, classMetaTag);
        } else if (kind == CXCursor_EnumDecl) {
            EnumInfo enumInfo;
            enumInfo.name = spellingStr;
            context.global.enums.emplace_back(std::move(enumInfo));
            VisitChildren(EnumVisitor, EnumInfo, cursor, context.global.enums.back());
            ApplyMetaFilter(context.global.enums, enumMetaTag);
        } else if (kind == CXCursor_Namespace) {
            NamespaceInfo namespaceInfo;
            namespaceInfo.name = spellingStr;
            context.namespaces.emplace_back(std::move(namespaceInfo));
            VisitChildren(NamespaceVisitor, NamespaceInfo, cursor, context.namespaces.back());
        }
        CleanUpAndContinueVisit;
    }
}

namespace MirrorTool {
    Parser::Parser(std::string inSourceFile, std::vector<std::string> inHeaderDirs) : sourceFile(std::move(inSourceFile)), headerDirs(std::move(inHeaderDirs))
    {
    }

    Parser::~Parser() = default;

    Parser::Result Parser::Parse() const
    {
        std::vector<std::string> argumentStrs = {
            "-x", "c++",
            "-std=c++20",
#if BUILD_CONFIG_DEBUG
            "-DBUILD_CONFIG_DEBUG=1",
#else
            "-DBUILD_CONFIG_DEBUG=0",
#endif
#if BUILD_EDITOR
            "-DBUILD_EDITOR=1",
#else
            "-DBUILD_EDITOR=0",
#endif
#if PLATFORM_WINDOWS
            "-DPLATFORM_WINDOWS=1",
            "-DNOMINMAX=1",
#elif PLATFORM_MACOS
            "-DPLATFORM_MACOS=1",
            fmt::format("-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX{}.sdk/usr/include/c++/v1", MACOS_SDK_VERSION),
            fmt::format("-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX{}.sdk/usr/include", MACOS_SDK_VERSION),
            fmt::format("-I/Library/Developer/CommandLineTools/usr/lib/clang/{}.{}.{}/include", __clang_major__, __clang_minor__, __clang_patchlevel__),
            fmt::format("-I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/{}.{}.{}/include", __clang_major__, __clang_minor__, __clang_patchlevel__),
#elif DPLATFORM_LINUX
            "-DPLATFORM_LINUX=1",
#endif
        };
        argumentStrs.reserve(argumentStrs.size() + headerDirs.size());
        for (const std::string& headerDir : headerDirs) {
            argumentStrs.emplace_back(std::string("-I") + headerDir);
        }

        std::vector<const char*> arguments(argumentStrs.size());
        for (auto i = 0; i < arguments.size(); i++) {
            arguments[i] = argumentStrs[i].c_str();
        }

        CXIndex index = clang_createIndex(0, 0);
        CXTranslationUnit translationUnit = clang_parseTranslationUnit(index, sourceFile.c_str(), arguments.data(), static_cast<int>(arguments.size()), nullptr, 0, CXTranslationUnit_None);
        if (translationUnit == nullptr) {
            return CleanUpAndConstructFailResult(index, translationUnit, "failed to create translation unit from source file");
        }

        uint32_t diagnosticsNum = clang_getNumDiagnostics(translationUnit);
        bool hasAnyError = false;
        std::stringstream errorInfos;
        for (auto i = 0; i < diagnosticsNum; i++) {
            CXDiagnostic diagnostic = clang_getDiagnostic(translationUnit, i);
            CXString diagnosticCXString = clang_formatDiagnostic(diagnostic, clang_defaultDiagnosticDisplayOptions());
            std::string diagnosticString = std::string(clang_getCString(diagnosticCXString));
            clang_disposeString(diagnosticCXString);

            if (diagnosticString.find("error: ") != std::string::npos) {
                hasAnyError = true;
                errorInfos << diagnosticString << '\n';
            }
        }
        if (hasAnyError) {
            return CleanUpAndConstructFailResult(index, translationUnit, errorInfos.str());
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
