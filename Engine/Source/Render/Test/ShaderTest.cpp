//
// Created by johnk on 2022/7/25.
//

#include <Test/Test.h>

#include <Render/Shader.h>

class TestGlobalShaderVS final : public Render::StaticShaderType<TestGlobalShaderVS> {
    ShaderTypeInfo(
        TestGlobalShaderVS,
        RHI::ShaderStageBits::sVertex,
        "Engine/Shader/Test/TestGlobalShader.esl",
        "VSMain")

    DeclBoolVariantField(TestBool, TEST_BOOL, false)
    DeclRangedIntVariantField(TestRangedInt, TEST_RANGED_INT, 0, 0, 3)
    MakeVariantFieldVec(TestBool, TestRangedInt)

    BeginIncludeDirectories
        "Engine/Shader/Test"
    EndIncludeDirectories
};

ImplementStaticShaderType(TestGlobalShaderVS)

class TestVertexFactory final : public Render::StaticVertexFactoryType<TestVertexFactory> {
    VertexFactoryTypeInfo(
        TestVertexFactory,
        "Engine/Shader/Test/VertexFactory.esh")

    DeclBoolVariantField(TestBool, TEST_BOOL, false)
    MakeVariantFieldVec(TestBool)

    DeclVertexInput(BaseColorInput, BaseColor, RHI::VertexFormat::float32X3, 0)
    MakeVertexInputVec(BaseColorInput)

    BeginSupportedMaterialTypes
        Render::MaterialType::surface
    EndSupportedMaterialTypes
};

ImplementStaticVertexFactoryType(TestVertexFactory)

TEST(ShaderTest, StaticShaderTypeTest)
{
    const auto& testGlobalShaderVS = TestGlobalShaderVS::Get();
    ASSERT_EQ(testGlobalShaderVS.GetName(), "TestGlobalShaderVS");
    ASSERT_EQ(testGlobalShaderVS.GetStage(), RHI::ShaderStageBits::sVertex);
    ASSERT_EQ(testGlobalShaderVS.GetSourceFile(), "Engine/Shader/Test/TestGlobalShader.esl");
    ASSERT_EQ(testGlobalShaderVS.GetEntryPoint(), "VSMain");

    const auto& resultIncludeDirectories = testGlobalShaderVS.GetIncludeDirectories();
    const std::vector<std::string> aspectIncludes = { "Engine/Shader/Test" };
    ASSERT_EQ(resultIncludeDirectories, aspectIncludes);

    const auto& resultVariantFields = testGlobalShaderVS.GetVariantFields();
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

TEST(ShaderTest, VertexFactoryTest)
{
    const auto& testVertexFactory = TestVertexFactory::Get();
    ASSERT_EQ(testVertexFactory.GetName(), "TestVertexFactory");
    ASSERT_EQ(testVertexFactory.GetSourceFile(), "Engine/Shader/Test/VertexFactory.esh");

    const auto& resultVariantFields = testVertexFactory.GetVariantFields();
    Render::ShaderVariantFieldVec aspectVariantFields;
    aspectVariantFields.emplace_back(Render::ShaderBoolVariantField { "TEST_BOOL", false });
    ASSERT_EQ(resultVariantFields, aspectVariantFields);

    const auto& resultInputs = testVertexFactory.GetVertexInputs();
    Render::VertexFactoryInputVec aspectInputs;
    aspectInputs.emplace_back(Render::VertexFactoryInput { "BaseColor", RHI::VertexFormat::float32X3, 0 });
    ASSERT_EQ(resultInputs, aspectInputs);

    const auto aspectSupportedMaterialTypes = std::unordered_set { Render::MaterialType::surface };
    for (auto i = 0; i < static_cast<uint8_t>(Render::MaterialType::max); i++) {
        const auto type = static_cast<Render::MaterialType>(i);
        ASSERT_EQ(testVertexFactory.SupportMaterialType(type), aspectSupportedMaterialTypes.contains(type));
    }
}
