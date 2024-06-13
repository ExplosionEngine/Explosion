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
        RHI::ShaderStageBits::sVertex);

    BoolShaderVariantField(TestBoolVariant, "TEST_BOOL");
    RangedIntShaderVariantField(TestRangedIntVariant, "TEST_RANGED_INT", 0, 3);
    VariantSet(TestBoolVariant, TestRangedIntVariant);

    DefaultVariantFilter
};
//RegisterGlobalShader(TestGlobalShaderVS);

TEST(ShaderTest, StaticVariantSetTest)
{
    ASSERT_EQ(TestGlobalShaderVS::VariantSet::VariantNum(), 8);

    const std::set<std::pair<bool, uint8_t>> expectVariants = {
        { false, 0 },
        { false, 1 },
        { false, 2 },
        { false, 3 },
        { true, 0 },
        { true, 1 },
        { true, 2 },
        { true, 3 }
    };
    std::vector<std::pair<bool, uint8_t>> actualVariants;
    TestGlobalShaderVS::VariantSet::TraverseAll([&actualVariants](auto&& variantSet) -> void {
        actualVariants.emplace_back(
            variantSet.template Get<TestGlobalShaderVS::TestBoolVariant>(),
            variantSet.template Get<TestGlobalShaderVS::TestRangedIntVariant>()
        );
    });

    ASSERT_EQ(expectVariants.size(), actualVariants.size());
    for (const auto& variant : actualVariants) {
        ASSERT_EQ(expectVariants.contains(variant), true);
    }
}
