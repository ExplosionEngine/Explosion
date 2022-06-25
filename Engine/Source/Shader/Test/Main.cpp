//
// Created by johnk on 2022/6/24.
//

#include <gtest/gtest.h>
#include <glm/glm.hpp>

#include <Common/Meta.h>
#include <Shader/Shader.h>

struct Meta(Class, VertexFactory)
TestVertexFactory {
    Meta(Property)
    glm::vec3 position;

    Meta(Property)
    glm::vec3 color;
};

struct Meta(Class, ShaderBuffer)
TestShaderBuffer {
    Meta(Property)
    glm::mat4 model;

    Meta(Property)
    glm::mat4 view;

    Meta(Property)
    glm::mat4 projection;
};

struct Meta(Class, ShaderParameterSet)
TestShaderParameterSet {
    Meta(Property, BufferType = "TestShaderBuffer")
    Shader::Buffer testShaderBuffer;

    Meta(Property)
    Shader::SamplerState testSamplerState;

    Meta(Property)
    Shader::Texture<float, 2> testTexture;
};

struct Meta(Class, ShaderVariantSet)
TestShaderVariantSet {
    Meta(Property, Macro = "TEST_BOOL")
    bool testBool = false;

    Meta(Property, Macro = "TEST_RANGED_INT", RangeFrom = 0, RangeTo = 2)
    int32_t testRangedInt = 0;
};

class TestShader : public Shader::Shader<TestVertexFactory, TestShaderParameterSet, TestShaderVariantSet> {
    static constexpr std::string_view sourceFile = "Shader/Test/TestShader.esl";
    static constexpr std::string_view entryPoint = "VSMain";
    static constexpr RHI::ShaderStageBits stage = RHI::ShaderStageBits::VERTEX;
};

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
