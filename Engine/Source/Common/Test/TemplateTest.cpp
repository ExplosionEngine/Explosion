//
// Created by johnk on 2026/6/20.
//

#include <Common/Template.h>
#include <Common/File.h>
#include <Common/FileSystem.h>
#include <Test/Test.h>

using namespace Common;

TEST(TemplateTest, BasicSubstitutionTest)
{
    TemplateEngine engine;
    engine.Set("name", "Explosion").Set("version", "2");

    const auto result = engine.Render("project({{name}} v{{version}})");
    ASSERT_TRUE(result.IsOk());
    ASSERT_EQ(result.Value(), "project(Explosion v2)");
}

TEST(TemplateTest, WhitespaceInsideTagTest)
{
    TemplateEngine engine;
    engine.Set("name", "Explosion");

    const auto result = engine.Render("{{ name }} and {{  name  }}");
    ASSERT_TRUE(result.IsOk());
    ASSERT_EQ(result.Value(), "Explosion and Explosion");
}

TEST(TemplateTest, BatchSetTest)
{
    TemplateEngine engine;
    engine.Set({ { "a", "1" }, { "b", "2" } });

    const auto result = engine.Render("{{a}}-{{b}}");
    ASSERT_TRUE(result.IsOk());
    ASSERT_EQ(result.Value(), "1-2");
}

TEST(TemplateTest, NoTagsTest)
{
    TemplateEngine engine;

    const auto result = engine.Render("plain text with no tags");
    ASSERT_TRUE(result.IsOk());
    ASSERT_EQ(result.Value(), "plain text with no tags");
}

TEST(TemplateTest, UndefinedVariableErrorsTest)
{
    TemplateEngine engine;
    engine.Set("known", "ok");

    const auto result = engine.Render("{{ known }}-{{ unknown }}");
    ASSERT_TRUE(result.IsErr());
    ASSERT_NE(result.Error().find("unknown"), std::string::npos);
}

TEST(TemplateTest, UnterminatedTagErrorsTest)
{
    TemplateEngine engine;

    const auto result = engine.Render("unterminated {{ name");
    ASSERT_TRUE(result.IsErr());
}

TEST(TemplateTest, HasAndClearTest)
{
    TemplateEngine engine;
    engine.Set("a", "1");
    ASSERT_TRUE(engine.Has("a"));
    ASSERT_FALSE(engine.Has("b"));
    engine.Clear();
    ASSERT_FALSE(engine.Has("a"));
}

TEST(TemplateTest, RenderFileTest)
{
    static Common::Path srcFile = "../Test/Generated/Common/TemplateSource.txt";
    static Common::Path dstFile = "../Test/Generated/Common/TemplateOutput.txt";

    ASSERT_TRUE(FileUtils::WriteTextFile(srcFile.Absolute().String(), "project({{name}})").IsOk());

    TemplateEngine engine;
    engine.Set("name", "Explosion");
    ASSERT_TRUE(engine.RenderFileTo(srcFile.Absolute().String(), dstFile.Absolute().String()).IsOk());

    const auto content = FileUtils::ReadTextFile(dstFile.Absolute().String());
    ASSERT_TRUE(content.IsOk());
    ASSERT_EQ(content.Value(), "project(Explosion)");
}

TEST(TemplateTest, RenderMissingFileErrorsTest)
{
    TemplateEngine engine;

    const auto result = engine.RenderFile("../Test/Generated/Common/DoesNotExist.tpl");
    ASSERT_TRUE(result.IsErr());
}
