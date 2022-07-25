//
// Created by johnk on 2022/7/25.
//

#include <gtest/gtest.h>

#include <Render/Shader.h>

class TestGlobalShader : public Render::GlobalShader {
public:
    GlobalShaderInfo("TestGlobalShader", "/Engine/Shader/TestGlobalShader.esl");

    StaticBoolShaderVariantField(TestBoolVariant, "TEST_BOOL");
    StaticRangedIntShaderVariantField(TestRangedIntVariant, "TEST_RANGED_INT", 0, 3);
    StaticVariantSet(TestBoolVariant, TestRangedIntVariant);

    DefaultStaticVariantFilter
};
RegisterGlobalShader(TestGlobalShader);

TEST(ShaderTest, StaticVariantSetTest)
{
    ASSERT_EQ(TestGlobalShader::VariantSet::VariantNum(), 8);
}
