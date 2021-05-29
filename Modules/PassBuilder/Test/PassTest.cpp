#include <gtest/gtest.h>
#include "PassLoader.h"

class PassLoaderTest : public testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PassLoaderTest, TestLoad) {
    GraphInfo graph = PassLoader::Load("test.json");
    ASSERT_EQ(graph.targets.size(), 4);

    ASSERT_EQ(graph.targets[0].width, 1024);
    ASSERT_EQ(graph.targets[0].height, 1024);

    ASSERT_EQ(graph.targets[1].width, 0);
    ASSERT_EQ(graph.targets[1].height, 0);

    EXPECT_EQ(0, 0);
}