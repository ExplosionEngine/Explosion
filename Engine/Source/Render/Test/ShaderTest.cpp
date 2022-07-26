//
// Created by johnk on 2022/7/25.
//

#include <gtest/gtest.h>

#include <Render/Shader.h>

class TestGlobalShader : public Render::GlobalShader {
public:
    ShaderInfo("TestGlobalShader", "/Engine/Shader/TestGlobalShader.esl");

    BoolShaderVariantField(TestBoolVariant, "TEST_BOOL");
    RangedIntShaderVariantField(TestRangedIntVariant, "TEST_RANGED_INT", 0, 3);
    VariantSet(TestBoolVariant, TestRangedIntVariant);

    DefaultVariantFilter
};
RegisterGlobalShader(TestGlobalShader);

TEST(ShaderTest, StaticVariantSetTest)
{
    ASSERT_EQ(TestGlobalShader::VariantSet::VariantNum(), 8);

    auto count = 0;
    TestGlobalShader::VariantSet::TraverseAll([&count](auto&&) -> void { count++; });
    ASSERT_EQ(count, 8);
}
