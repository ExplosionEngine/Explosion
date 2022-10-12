//
// Created by johnk on 2022/9/29.
//

#include <gtest/gtest.h>

#include <Mirror/Registry.h>

int v0 = 1;

int F0(int a, int b)
{
    return a + b;
}

TEST(MirrorTest, RegistryTest)
{
    Mirror::Registry::Get()
        .Global()
            .Variable<&v0>("v0")
                .Meta("editorToolTip", "this is a test value")
                .Meta("minValue", "0")
                .Meta("maxValue", "1")
            .Function<&F0>("F0")
                .Meta("editorBtn", "TestButton");
}
