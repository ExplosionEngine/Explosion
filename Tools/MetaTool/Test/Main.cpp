//
// Created by johnk on 2022/6/1.
//

#include <gtest/gtest.h>

#include <MetaTool/ClangParser.h>
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

void AssertStructClassContextEqual(const StructContext& a, const StructContext& b)
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

TEST(MetaToolTest, ClangParserGlobalTest)
{
    SourceInfo sourceInfo {};
    sourceInfo.sourceFile = "Test/MetaTool/GlobalTest.h";
    sourceInfo.includePathNum = includePaths.size();
    sourceInfo.includePaths = includePaths.data();

    ClangParser clangParser(sourceInfo);
    clangParser.Parse();
    const auto& metaContext = clangParser.GetMetaContext();

    ASSERT_EQ(metaContext.variables.size(), 5);
    AssertVariableContextEqual(metaContext.variables[0], VariableContext { "v0", "Property", "int" });
    AssertVariableContextEqual(metaContext.variables[1], VariableContext { "v1", "Property", "float" });
    AssertVariableContextEqual(metaContext.variables[2], VariableContext { "v2", "Property", "double" });
    AssertVariableContextEqual(metaContext.variables[3], VariableContext { "v3", "Property", "int *" });
    AssertVariableContextEqual(metaContext.variables[4], VariableContext { "v4", "Property", "int **" });

    ASSERT_EQ(metaContext.functions.size(), 4);
    AssertFunctionContextEqual(metaContext.functions[0], FunctionContext { "f0", "Function, EditorUI(Alias(\"TestButton\"))", "void", {} });
    AssertFunctionContextEqual(metaContext.functions[1], FunctionContext { "f1", "Function", "int", {} });
    AssertFunctionContextEqual(metaContext.functions[2], FunctionContext { "f2", "Function", "float", { ParamContext { "a", "int" }, ParamContext { "b", "int" } } });
    AssertFunctionContextEqual(metaContext.functions[3], FunctionContext { "f3", "Function", "double *", { ParamContext { "a", "int *" }, ParamContext { "b", "int *" } } });
}

TEST(MetaToolTest, ClangParserStructClassTest)
{
    SourceInfo sourceInfo {};
    sourceInfo.sourceFile = "Test/MetaTool/StructClassTest.h";
    sourceInfo.includePathNum = includePaths.size();
    sourceInfo.includePaths = includePaths.data();

    ClangParser clangParser(sourceInfo);
    clangParser.Parse();
    const auto& metaContext = clangParser.GetMetaContext();

    ASSERT_EQ(metaContext.structs.size(), 3);

    StructContext structContext {};
    structContext.metaData = "Struct";
    structContext.name = "S0";
    structContext.variables = {
        VariableContext { "a", "Property", "int" },
        VariableContext { "b", "Property", "float" },
        VariableContext { "c", "Property", "double" }
    };
    AssertStructClassContextEqual(metaContext.structs[0], structContext);

    structContext = StructContext {};
    structContext.metaData = "Struct";
    structContext.name = "S1";
    structContext.variables = {
        VariableContext { "c", "Property", "double" },
    };
    AssertStructClassContextEqual(metaContext.structs[1], structContext);

    structContext = StructContext {};
    structContext.metaData = "Struct";
    structContext.name = "S2";
    structContext.functions = {
        FunctionContext { "GetA", "Function", "int", {} },
        FunctionContext { "GetPointerB", "Function", "float *", { ParamContext { "t", "int" } } }
    };
    AssertStructClassContextEqual(metaContext.structs[2], structContext);

    ASSERT_EQ(metaContext.classes.size(), 1);

    ClassContext classContext {};
    classContext.metaData = "Class";
    classContext.name = "C0";
    classContext.functions = {
        FunctionContext { "GetA", "Function", "int *", { ParamContext { "b", "float *" } } }
    };
    AssertStructClassContextEqual(metaContext.classes[0], classContext);
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

    ASSERT_EQ(metaContext.name, "Global");

    ASSERT_EQ(metaContext.functions.size(), 1);
    AssertFunctionContextEqual(metaContext.functions[0], FunctionContext { "F0", "Function", "int", {} });

    ASSERT_EQ(metaContext.namespaces.size(), 1);
    ASSERT_EQ(metaContext.namespaces[0].name, "N0");

    ASSERT_EQ(metaContext.namespaces[0].variables.size(), 1);
    AssertVariableContextEqual(metaContext.namespaces[0].variables[0], VariableContext { "v0", "Property", "int" });

    ASSERT_EQ(metaContext.namespaces[0].functions.size(), 1);
    AssertFunctionContextEqual(metaContext.namespaces[0].functions[0], FunctionContext { "F1", "Function", "float", { ParamContext { "a", "int" } } });

    ASSERT_EQ(metaContext.namespaces[0].classes.size(), 1);

    ClassContext classContext {};
    classContext.metaData = "Class";
    classContext.name = "C0";
    classContext.functions = {
        FunctionContext { "GetA", "Function", "int **", {} }
    };
    AssertStructClassContextEqual(metaContext.namespaces[0].classes[0], classContext);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
