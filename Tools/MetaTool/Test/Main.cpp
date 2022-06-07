//
// Created by johnk on 2022/6/1.
//

#include <gtest/gtest.h>

#include <MetaTool/ClangParser.h>

TEST(MetaToolTest, ClangParserGlobalTest)
{
    MetaTool::SourceInfo sourceInfo {};
    sourceInfo.sourceFile = "Test/MetaTool/GlobalTest.h";

    MetaTool::ClangParser clangParser(sourceInfo);
    clangParser.Parse();
    const auto& metaContext = clangParser.GetMetaContext();

    ASSERT_EQ(metaContext.variables[0].name, "v0");
    ASSERT_EQ(metaContext.variables[0].type, "int");

    ASSERT_EQ(metaContext.variables[1].name, "v1");
    ASSERT_EQ(metaContext.variables[1].type, "float");

    ASSERT_EQ(metaContext.variables[2].name, "v2");
    ASSERT_EQ(metaContext.variables[2].type, "double");

    ASSERT_EQ(metaContext.variables[3].name, "v3");
    ASSERT_EQ(metaContext.variables[3].type, "int *");

    ASSERT_EQ(metaContext.variables[4].name, "v4");
    ASSERT_EQ(metaContext.variables[4].type, "int **");

    ASSERT_EQ(metaContext.functions[0].name, "f0");
    ASSERT_EQ(metaContext.functions[0].prototype, "void ()");
    ASSERT_EQ(metaContext.functions[0].returnType, "void");
    ASSERT_EQ(metaContext.functions[0].paramNames.size(), 0);
    ASSERT_EQ(metaContext.functions[0].paramTypes.size(), 0);

    ASSERT_EQ(metaContext.functions[1].name, "f1");
    ASSERT_EQ(metaContext.functions[1].prototype, "int ()");
    ASSERT_EQ(metaContext.functions[1].returnType, "int");
    ASSERT_EQ(metaContext.functions[1].paramNames.size(), 0);
    ASSERT_EQ(metaContext.functions[1].paramTypes.size(), 0);

    ASSERT_EQ(metaContext.functions[2].name, "f2");
    ASSERT_EQ(metaContext.functions[2].prototype, "float (int, int)");
    ASSERT_EQ(metaContext.functions[2].returnType, "float");
    ASSERT_EQ(metaContext.functions[2].paramNames.size(), 2);
    ASSERT_EQ(metaContext.functions[2].paramNames[0], "a");
    ASSERT_EQ(metaContext.functions[2].paramNames[1], "b");
    ASSERT_EQ(metaContext.functions[2].paramTypes.size(), 2);
    ASSERT_EQ(metaContext.functions[2].paramTypes[0], "int");
    ASSERT_EQ(metaContext.functions[2].paramTypes[1], "int");

    ASSERT_EQ(metaContext.functions[3].name, "f3");
    ASSERT_EQ(metaContext.functions[3].prototype, "double *(int *, int *)");
    ASSERT_EQ(metaContext.functions[3].returnType, "double *");
    ASSERT_EQ(metaContext.functions[3].paramNames.size(), 2);
    ASSERT_EQ(metaContext.functions[3].paramNames[0], "a");
    ASSERT_EQ(metaContext.functions[3].paramNames[1], "b");
    ASSERT_EQ(metaContext.functions[3].paramTypes.size(), 2);
    ASSERT_EQ(metaContext.functions[3].paramTypes[0], "int *");
    ASSERT_EQ(metaContext.functions[3].paramTypes[1], "int *");
}

TEST(MetaToolTest, ClangParserStructTest)
{
    MetaTool::SourceInfo sourceInfo {};
    sourceInfo.sourceFile = "Test/MetaTool/StructTest.h";

    MetaTool::ClangParser clangParser(sourceInfo);
    clangParser.Parse();
    const auto& metaContext = clangParser.GetMetaContext();

    ASSERT_EQ(metaContext.structs[0].name, "S0");
    ASSERT_EQ(metaContext.structs[0].variables.size(), 3);
    ASSERT_EQ(metaContext.structs[0].variables[0].accessSpecifier, MetaTool::AccessSpecifier::DEFAULT);
    ASSERT_EQ(metaContext.structs[0].variables[0].name, "a");
    ASSERT_EQ(metaContext.structs[0].variables[0].type, "int");
    ASSERT_EQ(metaContext.structs[0].variables[1].accessSpecifier, MetaTool::AccessSpecifier::DEFAULT);
    ASSERT_EQ(metaContext.structs[0].variables[1].name, "b");
    ASSERT_EQ(metaContext.structs[0].variables[1].type, "float");
    ASSERT_EQ(metaContext.structs[0].variables[2].accessSpecifier, MetaTool::AccessSpecifier::DEFAULT);
    ASSERT_EQ(metaContext.structs[0].variables[2].name, "c");
    ASSERT_EQ(metaContext.structs[0].variables[2].type, "double");

    ASSERT_EQ(metaContext.structs[1].name, "S1");
    ASSERT_EQ(metaContext.structs[1].variables.size(), 3);
    ASSERT_EQ(metaContext.structs[1].variables[0].accessSpecifier, MetaTool::AccessSpecifier::PUBLIC);
    ASSERT_EQ(metaContext.structs[1].variables[0].name, "c");
    ASSERT_EQ(metaContext.structs[1].variables[0].type, "double");
    ASSERT_EQ(metaContext.structs[1].variables[1].accessSpecifier, MetaTool::AccessSpecifier::PROTECTED);
    ASSERT_EQ(metaContext.structs[1].variables[1].name, "b");
    ASSERT_EQ(metaContext.structs[1].variables[1].type, "float");
    ASSERT_EQ(metaContext.structs[1].variables[2].accessSpecifier, MetaTool::AccessSpecifier::PRIVATE);
    ASSERT_EQ(metaContext.structs[1].variables[2].name, "a");
    ASSERT_EQ(metaContext.structs[1].variables[2].type, "int");

    ASSERT_EQ(metaContext.structs[2].name, "S2");
    ASSERT_EQ(metaContext.structs[2].variables.size(), 2);
    ASSERT_EQ(metaContext.structs[2].variables[0].accessSpecifier, MetaTool::AccessSpecifier::PRIVATE);
    ASSERT_EQ(metaContext.structs[2].variables[0].name, "a");
    ASSERT_EQ(metaContext.structs[2].variables[0].type, "int");
    ASSERT_EQ(metaContext.structs[2].variables[1].accessSpecifier, MetaTool::AccessSpecifier::PRIVATE);
    ASSERT_EQ(metaContext.structs[2].variables[1].name, "b");
    ASSERT_EQ(metaContext.structs[2].variables[1].type, "float");
    ASSERT_EQ(metaContext.structs[2].functions.size(), 2);
    ASSERT_EQ(metaContext.structs[2].functions[0].accessSpecifier, MetaTool::AccessSpecifier::PUBLIC);
    ASSERT_EQ(metaContext.structs[2].functions[0].name, "GetA");
    ASSERT_EQ(metaContext.structs[2].functions[0].prototype, "int ()");
    ASSERT_EQ(metaContext.structs[2].functions[0].returnType, "int");
    ASSERT_EQ(metaContext.structs[2].functions[0].paramNames.size(), 0);
    ASSERT_EQ(metaContext.structs[2].functions[0].paramTypes.size(), 0);
    ASSERT_EQ(metaContext.structs[2].functions[1].accessSpecifier, MetaTool::AccessSpecifier::PUBLIC);
    ASSERT_EQ(metaContext.structs[2].functions[1].name, "GetPointerB");
    ASSERT_EQ(metaContext.structs[2].functions[1].prototype, "float *(int)");
    ASSERT_EQ(metaContext.structs[2].functions[1].returnType, "float *");
    ASSERT_EQ(metaContext.structs[2].functions[1].paramNames.size(), 1);
    ASSERT_EQ(metaContext.structs[2].functions[1].paramNames[0], "t");
    ASSERT_EQ(metaContext.structs[2].functions[1].paramTypes.size(), 1);
    ASSERT_EQ(metaContext.structs[2].functions[1].paramTypes[0], "int");
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
