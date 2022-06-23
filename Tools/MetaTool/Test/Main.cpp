//
// Created by johnk on 2022/6/1.
//

#include <gtest/gtest.h>

#include <MetaTool/ClangParser.h>
#include <MetaTool.Test/ClassTest.meta.h>
using namespace MetaTool;

std::vector<const char*> includePaths = { "Test/MetaTool/Include" };

void AssertVariableContextEqual(const VariableContext& a, const VariableContext& b)
{
    ASSERT_EQ(a.name, b.name);
    ASSERT_EQ(a.metaData, b.metaData);
    ASSERT_EQ(a.type, b.type);
}

void AssertFunctionContextEqual(const FunctionContext& a, const FunctionContext& b)
{
    ASSERT_EQ(a.name, b.name);
    ASSERT_EQ(a.metaData, b.metaData);
    ASSERT_EQ(a.returnType, b.returnType);
    ASSERT_EQ(a.params.size(), b.params.size());
    for (auto i = 0; i < a.params.size(); i++) {
        ASSERT_EQ(a.params[i].name, b.params[i].name);
        ASSERT_EQ(a.params[i].type, b.params[i].type);
    }
}

void AssertClassContextEqual(const ClassContext& a, const ClassContext& b)
{
    ASSERT_EQ(a.name, b.name);
    ASSERT_EQ(a.metaData, b.metaData);
    ASSERT_EQ(a.variables.size(), b.variables.size());
    for (auto i = 0; i < a.variables.size(); i++) {
        AssertVariableContextEqual(a.variables[i], b.variables[i]);
    }
    ASSERT_EQ(a.functions.size(), b.functions.size());
    for (auto i = 0; i < a.functions.size(); i++) {
        AssertFunctionContextEqual(a.functions[i], b.functions[i]);
    }
}

TEST(MetaToolTest, ClangParserClassTest)
{
    SourceInfo sourceInfo {};
    sourceInfo.sourceFile = "Test/MetaTool/ClassTest.h";
    sourceInfo.includePathNum = includePaths.size();
    sourceInfo.includePaths = includePaths.data();

    ClangParser clangParser(sourceInfo);
    clangParser.Parse();
    const auto& metaContext = clangParser.GetMetaContext();

    ASSERT_EQ(metaContext.classes.size(), 4);

    ClassContext classContext {};
    classContext.metaData = "Class";
    classContext.name = "S0";
    classContext.variables = {
        VariableContext { "a", "Property", "int" },
        VariableContext { "b", "Property", "float" },
        VariableContext { "c", "Property", "double" }
    };
    AssertClassContextEqual(metaContext.classes[0], classContext);

    classContext = ClassContext {};
    classContext.metaData = "Class";
    classContext.name = "S1";
    classContext.variables = {
        VariableContext { "c", "Property", "double" },
    };
    AssertClassContextEqual(metaContext.classes[1], classContext);

    classContext = ClassContext {};
    classContext.metaData = "Class";
    classContext.name = "S2";
    classContext.functions = {
        FunctionContext { "GetA", "Function", "int", {} },
        FunctionContext { "GetPointerB", "Function", "float *", { ParamContext { "t", "int" } } }
    };
    AssertClassContextEqual(metaContext.classes[2], classContext);

    classContext = ClassContext {};
    classContext.metaData = "Class";
    classContext.name = "C0";
    classContext.functions = {
        FunctionContext { "GetA", "Function", "int *", { ParamContext { "b", "float *" } } }
    };
    AssertClassContextEqual(metaContext.classes[3], classContext);
}

TEST(MetaToolTest, ClangParserNamespaceTest)
{
    SourceInfo sourceInfo {};
    sourceInfo.sourceFile = "Test/MetaTool/NamespaceTest.h";
    sourceInfo.includePathNum = includePaths.size();
    sourceInfo.includePaths = includePaths.data();

    ClangParser clangParser(sourceInfo);
    clangParser.Parse();
    const auto& metaContext = clangParser.GetMetaContext();

    ASSERT_EQ(metaContext.namespaces.size(), 1);
    ASSERT_EQ(metaContext.namespaces[0].name, "N0");

    ASSERT_EQ(metaContext.namespaces[0].classes.size(), 1);

    ClassContext classContext {};
    classContext.metaData = "Class";
    classContext.name = "C0";
    classContext.functions = {
        FunctionContext { "GetA", "Function", "int **", {} }
    };
    AssertClassContextEqual(metaContext.namespaces[0].classes[0], classContext);
}

TEST(MetaToolTest, HeaderGeneratorClassTest)
{
    std::hash<std::string_view> hash {};

    {
        meta::type s0 = meta::resolve(hash("S0"));
        ASSERT_EQ(s0.prop(hash("name")).value().cast<std::string_view>(), "S0");

        meta::data a = s0.data(hash("a"));
        meta::data b = s0.data(hash("b"));
        meta::data c = s0.data(hash("c"));
        ASSERT_EQ(a.prop(hash("name")).value().cast<std::string_view>(), "a");
        ASSERT_EQ(b.prop(hash("name")).value().cast<std::string_view>(), "b");
        ASSERT_EQ(c.prop(hash("name")).value().cast<std::string_view>(), "c");

        meta::any instance = S0 { 1, 2.0f, 3.0 };
        ASSERT_EQ(a.get(instance).cast<int>(), 1);
        ASSERT_EQ(b.get(instance).cast<float>(), 2.0f);
        ASSERT_EQ(c.get(instance).cast<double>(), 3.0);
    }

    {
        meta::type s1 = meta::resolve(hash("S1"));
        ASSERT_EQ(s1.prop(hash("name")).value().cast<std::string_view>(), "S1");

        meta::ctor s1Ctor = s1.ctor<int, float, double>();
        meta::data c = s1.data(hash("c"));
        ASSERT_EQ(c.prop(hash("name")).value().cast<std::string_view>(), "c");

        meta::any instance = s1Ctor.invoke(1, 2.0f, 3.0);
        ASSERT_EQ(c.get(instance).cast<double>(), 3.0);
    }

    {
        meta::type s2 = meta::resolve(hash("S2"));
        ASSERT_EQ(s2.prop(hash("name")).value().cast<std::string_view>(), "S2");

        meta::ctor s2Ctor = s2.ctor<int, float>();
        meta::func getA = s2.func(hash("GetA"));
        ASSERT_EQ(getA.prop(hash("name")).value().cast<std::string_view>(), "GetA");

        meta::any instance = s2Ctor.invoke(1, 2.0f);
        ASSERT_EQ(getA.invoke(instance).cast<int>(), 1);
    }
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
