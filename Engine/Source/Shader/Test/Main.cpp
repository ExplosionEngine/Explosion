//
// Created by johnk on 2022/6/24.
//

#include <gtest/gtest.h>

#include "Shader/EngineShader.h"

class TestEngineShader : public Shader::EngineShader {
public:
    class TestBoolVariant : public BoolVariant("TEST_BOOL");
    class TestRangedIntVariant : public RangedIntVariant("TEST_RANGED_INT", 0, 3);
    class VariantSet : public Shader::VariantSet<TestBoolVariant, TestRangedIntVariant> {};
};

TEST(ShaderTest, EngineShaderTest)
{
    // TODO
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
