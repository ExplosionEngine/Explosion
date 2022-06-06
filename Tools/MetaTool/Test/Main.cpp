//
// Created by johnk on 2022/6/1.
//

#include <gtest/gtest.h>

#include <MetaTool/ClangParser.h>

TEST(MetaToolTest, ClangParserStructTest)
{
    MetaTool::SourceInfo sourceInfo {};
    sourceInfo.sourceFile = "Test.h";

    MetaTool::ClangParser clangParser(sourceInfo);
    clangParser.Parse();

    // TODO
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
