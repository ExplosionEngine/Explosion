//
// Created by johnk on 2022/7/25.
//

#include <gtest/gtest.h>

#include <Render/Shader.h>

class TestGlobalShaderVS : public Render::GlobalShader {
public:
    ShaderInfo(
        "TestGlobalShader",
        "/Engine/Shader/Test/TestGlobalShader.esl",
        "VSMain",
        Render::ShaderStage::S_VERTEX);

    BoolShaderVariantField(TestBoolVariant, "TEST_BOOL");
    RangedIntShaderVariantField(TestRangedIntVariant, "TEST_RANGED_INT", 0, 3);
    VariantSet(TestBoolVariant, TestRangedIntVariant);

    DefaultVariantFilter
};
RegisterGlobalShader(TestGlobalShaderVS);

TEST(ShaderTest, StaticVariantSetTest)
{
    ASSERT_EQ(TestGlobalShaderVS::VariantSet::VariantNum(), 8);

    auto count = 0;
    TestGlobalShaderVS::VariantSet::TraverseAll([&count](auto&&) -> void { count++; });
    ASSERT_EQ(count, 8);
}
