//
// Created by johnk on 2022/7/25.
//

#include <Test/Test.h>

#include <Render/Shader.h>

class TestGlobalShaderVS final : public Render::StaticShaderType<TestGlobalShaderVS> {
    StaticShaderInfo(
        TestGlobalShaderVS,
        "TestGlobalShader",
        RHI::ShaderStageBits::sVertex,
        "Engine/Shader/Test/TestGlobalShader.esl",
        "VSMain")

    BoolVariantField(TestBool, TEST_BOOL, false);
    RangedIntVariantField(TestRangedInt, TEST_RANGED_INT, 0, 0, 3);
    MakeVariantFieldVec(TestBool, TestRangedInt);

    BeginIncludeDirectories
        "Engine/Shader/Test"
    EndIncludeDirectories
};

ImplementStaticShaderType(TestGlobalShaderVS)

TEST(ShaderTest, StaticIncludePathAndVariantFieldsTest)
{
    const std::vector<std::string> aspectIncludes = { "Engine/Shader/Test" };
    ASSERT_EQ(TestGlobalShaderVS::Get().GetIncludeDirectories(), aspectIncludes);

    const auto resultVariantFields = TestGlobalShaderVS::Get().GetVariantFields();
    Render::ShaderVariantFieldVec aspectVariantFields;
    aspectVariantFields.emplace_back(Render::ShaderBoolVariantField { "TEST_BOOL", false });
    aspectVariantFields.emplace_back(Render::ShaderRangedIntVariantField { "TEST_RANGED_INT", 0, { 0, 3 } });
    ASSERT_EQ(resultVariantFields, aspectVariantFields);
}

TEST(ShaderTest, ComputeVariantKeyTest)
{
    const Render::ShaderVariantFieldVec variantFields = {
        Render::ShaderBoolVariantField { "TEST_BOOL", false },
        Render::ShaderRangedIntVariantField { "TEST_RANGED_INT", 1, { 1, 4 } }
    };
    const Render::ShaderVariantValueMap variantSet = {
        { "TEST_BOOL", true },
        { "TEST_RANGED_INT", 2 }
    };
    ASSERT_EQ(Render::ShaderUtils::ComputeVariantKey(variantFields, variantSet), 3);
}

TEST(ShaderTest, GetAllVariantsTest)
{
    const Render::ShaderVariantFieldVec variantFields = {
        Render::ShaderBoolVariantField { "TEST_BOOL", false },
        Render::ShaderRangedIntVariantField { "TEST_RANGED_INT", 1, { 1, 4 } }
    };

    const auto resultVariants = Render::ShaderUtils::GetAllVariants(variantFields);
    const std::vector<Render::ShaderVariantValueMap> aspectVariants = {
        { { "TEST_BOOL", false }, { "TEST_RANGED_INT", 1 } },
        { { "TEST_BOOL", true }, { "TEST_RANGED_INT", 1 } },
        { { "TEST_BOOL", false }, { "TEST_RANGED_INT", 2 } },
        { { "TEST_BOOL", true }, { "TEST_RANGED_INT", 2 } },
        { { "TEST_BOOL", false }, { "TEST_RANGED_INT", 3 } },
        { { "TEST_BOOL", true }, { "TEST_RANGED_INT", 3 } },
        { { "TEST_BOOL", false }, { "TEST_RANGED_INT", 4 } },
        { { "TEST_BOOL", true }, { "TEST_RANGED_INT", 4 } },
    };
    ASSERT_EQ(resultVariants, aspectVariants);
}

TEST(ShaderTest, ComputeVariantDefinitionsTest)
{
    const Render::ShaderVariantFieldVec variantFields = {
        Render::ShaderBoolVariantField { "TEST_BOOL", false },
        Render::ShaderRangedIntVariantField { "TEST_RANGED_INT", 1, { 1, 4 } }
    };
    const Render::ShaderVariantValueMap variantSet = {
        { "TEST_BOOL", true },
        { "TEST_RANGED_INT", 2 }
    };
    const std::vector<std::string> definitions = {
        "TEST_BOOL=1",
        "TEST_RANGED_INT=2"
    };
    ASSERT_EQ(Render::ShaderUtils::ComputeVariantDefinitions(variantFields, variantSet), definitions);
}
