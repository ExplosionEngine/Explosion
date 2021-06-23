//
// Created by John Kindem on 2021/6/24.
//

#include <gtest/gtest.h>

#include <Explosion/ECS/ECS.h>
#include <Explosion/World/World.h>

using namespace Explosion;

TEST(WorldTest, AddSystemGroupTest)
{
    ECS::SystemGroup systemGroup1;
    systemGroup1.priority = 1;
    ECS::SystemGroup systemGroup2;
    systemGroup2.priority = 5;
    ECS::SystemGroup systemGroup3;
    systemGroup3.priority = 3;
    ECS::SystemGroup systemGroup4;
    systemGroup4.priority = 10;

    World world;
    world.AddSystemGroup(systemGroup1);
    world.AddSystemGroup(systemGroup2);
    world.AddSystemGroup(systemGroup3);
    world.AddSystemGroup(systemGroup4);

    const auto& systemGroups = world.GetSystemGroups();
    ASSERT_EQ(systemGroup1.priority, 10);
    ASSERT_EQ(systemGroup2.priority, 5);
    ASSERT_EQ(systemGroup3.priority, 3);
    ASSERT_EQ(systemGroup4.priority, 1);
}
