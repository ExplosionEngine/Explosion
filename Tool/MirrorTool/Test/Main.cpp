//
// Created by johnk on 2022/12/12.
//

#include <gtest/gtest.h>

#include <MirrorTool/Parser.h>
#include <MirrorTool/Generator.h>

using namespace MirrorTool;

void AssertMetaDatasEqual(const MetaDataMap& lhs, const MetaDataMap& rhs)
{
    ASSERT_EQ(lhs.size(), rhs.size());
    for (const auto& [key, value] : lhs) {
        auto rhsIter = rhs.find(key);
        ASSERT_NE(rhsIter, rhs.end());
        ASSERT_EQ(value, rhsIter->second);
    }
}

template <typename T, typename F>
void AssertVectorEqual(const std::vector<T>& lhs, const std::vector<T>& rhs, F&& elementCompAssertion) // NOLINT
{
    ASSERT_EQ(lhs.size(), rhs.size());
    for (auto i = 0; i < lhs.size(); i++) {
        elementCompAssertion(lhs[i], rhs[i]);
    }
}

void AssertNodeEqual(const Node& lhs, const Node& rhs)
{
    ASSERT_EQ(lhs.name, rhs.name);
    ASSERT_EQ(rhs.outerName, rhs.outerName);
    AssertMetaDatasEqual(lhs.metaDatas, rhs.metaDatas);
}

void AssertVariableInfoEqual(const VariableInfo& lhs, const VariableInfo& rhs)
{
    AssertNodeEqual(lhs, rhs);
    ASSERT_EQ(lhs.type, rhs.type);
}

void AssertParametersEqual(const std::vector<ParamNameAndType>& lhs, const std::vector<ParamNameAndType>& rhs)
{
    AssertVectorEqual(lhs, rhs, [](const ParamNameAndType& elhs, const ParamNameAndType& erhs) -> void {
        ASSERT_EQ(elhs.first, erhs.first);
        ASSERT_EQ(elhs.second, erhs.second);
    });
}

void AssertFunctionInfoEqual(const FunctionInfo& lhs, const FunctionInfo& rhs)
{
    AssertNodeEqual(lhs, rhs);
    ASSERT_EQ(lhs.retType, rhs.retType);
    AssertParametersEqual(lhs.parameters, rhs.parameters);
}

void AssertClassVariableInfoEqual(const ClassVariableInfo& lhs, const ClassVariableInfo& rhs)
{
    AssertVariableInfoEqual(lhs, rhs);
    ASSERT_EQ(lhs.fieldAccess, rhs.fieldAccess);
}

void AssertClassFunctionInfoEqual(const ClassFunctionInfo& lhs, const ClassFunctionInfo& rhs)
{
    AssertFunctionInfoEqual(lhs, rhs);
    ASSERT_EQ(lhs.fieldAccess, rhs.fieldAccess);
}

void AssertClassVariableInfoVectorEqual(const std::vector<ClassVariableInfo>& lhs, const std::vector<ClassVariableInfo>& rhs)
{
    AssertVectorEqual(lhs, rhs, [](const ClassVariableInfo& elhs, const ClassVariableInfo& erhs) -> void {
        AssertClassVariableInfoEqual(elhs, erhs);
    });
}

void AssertClassFunctionInfoVectorEqual(const std::vector<ClassFunctionInfo>& lhs, const std::vector<ClassFunctionInfo>& rhs)
{
    AssertVectorEqual(lhs, rhs, [](const ClassFunctionInfo& elhs, const ClassFunctionInfo& erhs) -> void {
        AssertClassFunctionInfoEqual(elhs, erhs);
    });
}

void AssertClassInfoEqual(const ClassInfo& lhs, const ClassInfo& rhs)
{
    AssertNodeEqual(lhs, rhs);
    AssertClassVariableInfoVectorEqual(lhs.staticVariables, rhs.staticVariables);
    AssertClassFunctionInfoVectorEqual(lhs.staticFunctions, rhs.staticFunctions);
    AssertClassVariableInfoVectorEqual(lhs.variables, rhs.variables);
    AssertClassFunctionInfoVectorEqual(lhs.functions, rhs.functions);
}

void AssertEnumInfoEqual(const EnumInfo& lhs, const EnumInfo& rhs)
{
    AssertVectorEqual(lhs.elements, rhs.elements, [](const EnumElementInfo& elhs, const EnumElementInfo& erhs) -> void {
        AssertNodeEqual(elhs, erhs);
    });
}

void AssertNamespaceInfoEqual(const NamespaceInfo& lhs, const NamespaceInfo& rhs) // NOLINT
{
    AssertNodeEqual(lhs, rhs);
    AssertVectorEqual(lhs.variables, rhs.variables, [](const VariableInfo& elhs, const VariableInfo& erhs) -> void {
        AssertVariableInfoEqual(elhs, erhs);
    });
    AssertVectorEqual(lhs.functions, rhs.functions, [](const FunctionInfo& elhs, const FunctionInfo& erhs) -> void {
        AssertFunctionInfoEqual(elhs, erhs);
    });
    AssertVectorEqual(lhs.classes, rhs.classes, [](const ClassInfo& elhs, const ClassInfo& erhs) -> void {
        AssertClassInfoEqual(elhs, erhs);
    });
    AssertVectorEqual(lhs.namespaces, rhs.namespaces, [](const NamespaceInfo& elhs, const NamespaceInfo& erhs) -> void { // NOLINT
        AssertNamespaceInfoEqual(elhs, erhs);
    });
    AssertVectorEqual(lhs.enums, rhs.enums, [](const EnumInfo& elhs, const EnumInfo& erhs) -> void {
        AssertEnumInfoEqual(elhs, erhs);
    });
}

TEST(MirrorTest, ParserTest)
{
    const Parser parser("../Test/Resource/Mirror/MirrorToolInput.h", { "../Test/Resource/Mirror" });
    auto [parseSuccess, parseResultOrError] = parser.Parse();
    ASSERT_TRUE(parseSuccess);

    const auto& [namespaces, global] = std::get<MetaInfo>(parseResultOrError);
    ASSERT_EQ(namespaces.size(), 0);

    NamespaceInfo predicatedGlobalNamespace = { "", "", {} };
    predicatedGlobalNamespace.variables = {
        { "", "gv0", {}, "int" },
        { "", "gv1", {}, "float" }
    };
    predicatedGlobalNamespace.functions = {
        { "", "gf0", {}, "int", { { "a", "int" }, { "b", "int" } } }
    };
    predicatedGlobalNamespace.classes = { { "", "C0", {} } };
    predicatedGlobalNamespace.classes[0].staticVariables = {
        { "C0", "sv0", {}, "int", FieldAccess::pub }
    };
    predicatedGlobalNamespace.classes[0].staticFunctions = {
        { "C0", "sf0", {}, "void", {}, FieldAccess::pub }
    };
    predicatedGlobalNamespace.classes[0].variables = {
        { "C0", "v0", {{ "editorHide", "true" }}, "int", FieldAccess::pub },
        { "C0", "v1", {}, "float", FieldAccess::pub }
    };
    predicatedGlobalNamespace.classes[0].functions = {
        { "C0", "f0", {}, "int", {}, FieldAccess::pub }
    };
    predicatedGlobalNamespace.enums = { { "", "TestEnum", {} } };
    predicatedGlobalNamespace.enums[0].elements = {
        { "TestEnum", "a", {} },
        { "TestEnum", "b", {} },
        { "TestEnum", "max", {} }
    };
    AssertNamespaceInfoEqual(global, predicatedGlobalNamespace);
}

TEST(MirrorTest, GeneratorTest)
{
    const Parser parser("../Test/Resource/Mirror/MirrorToolInput.h", { "../Test/Resource/Mirror" });
    auto [parseSuccess, parseResultOrError] = parser.Parse();
    ASSERT_TRUE(parseSuccess);

    const Generator generator("../Test/Resource/Mirror/MirrorToolInput.h", "../Test/Generated/Mirror/MirrorToolTest.generated.cpp", { "../" }, std::get<MetaInfo>(parseResultOrError), false);
    auto [generateSuccess, generateResultOrError] = generator.Generate();
    ASSERT_EQ(generateSuccess, true);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
