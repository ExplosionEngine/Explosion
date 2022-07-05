//
// Created by johnk on 2022/6/29.
//

#pragma once

#include <glm/glm.hpp>

#include <Common/Meta.h>
#include <Shader/Shader.h>

struct Meta(Class) TestVertexFactory {
    Meta(Property) glm::vec3 position;
    Meta(Property) glm::vec3 color;
};

struct Meta(Class) TestShaderBuffer {
    Meta(Property) glm::mat4 model;
    Meta(Property) glm::mat4 view;
    Meta(Property) glm::mat4 projection;
};

struct Meta(Class) TestParameterSet {
    Meta(Property, BufferType = "TestShaderBuffer") Shader::Buffer testShaderBuffer;
    Meta(Property) Shader::SamplerState testSamplerState;
    Meta(Property) Shader::Texture<float, 2> testTexture;
};

struct Meta(Class) TestVariantSet {
    Meta(Property, Macro = "TEST_BOOL") bool testBool = false;
    Meta(Property, ShaderMacro = "TEST_RANGED_INT", MinValue = 0, MaxValue = 2) int32_t testRangedInt = 0;
};

class TestShader : public Shader::EngineShader {
public:
    static constexpr std::string_view sourceFile = "Shader/Test/TestShader.esl";
    static constexpr std::string_view entryPoint = "VSMain";
    static constexpr RHI::ShaderStageBits stage = RHI::ShaderStageBits::VERTEX;
    using VertexFactory = TestVertexFactory;
    using ParameterSet = TestParameterSet;
    using VariantSet = TestVariantSet;
};
RegisterEngineShader(TestShader);
