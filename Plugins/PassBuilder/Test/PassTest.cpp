#include "gtest/gtest.h"
#include "PassLoader.h"

class PassLoaderTest : public testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PassLoaderTest, TestLoad) {
    GraphInfo graph = PassLoader::Load("../../../Plugins/PassBuilder/Test/test.json");

    EXPECT_EQ(0, 0);
}