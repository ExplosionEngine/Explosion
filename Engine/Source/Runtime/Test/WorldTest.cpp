//
// Created by johnk on 2022/8/15.
//

#include <gtest/gtest.h>

#include <Runtime/World.h>
#include <WorldTest.h>

TEST(WorldTest, ComponentBasicTest)
{
    Runtime::World world("TestWorld");

    auto entity = world.CreateEntity();
    world.AddComponent<Position>(entity, 1.0f, 2.0f);

    auto* position = world.GetComponent<Position>(entity);
    ASSERT_NE(position, nullptr);
    ASSERT_EQ(position->x, 1.0f);
    ASSERT_EQ(position->y, 2.0f);

    world.RemoveComponent<Position>(entity);
    position = world.GetComponent<Position>(entity);
    ASSERT_EQ(position, nullptr);
}

TEST(WorldTest, SystemBasicTest)
{
    Runtime::World world("SystemBasicTestWorld");

    auto entity0 = world.CreateEntity();
    world.AddComponent<Position>(entity0, 1.0f, 2.0f);
    world.AddComponent<Velocity>(entity0, 0.0f, 0.0f);

    auto entity1 = world.CreateEntity();
    world.AddComponent<Position>(entity1, 5.0f, 6.0f);
    world.AddComponent<Velocity>(entity1, 1.0f, 2.0f);

    world.MountSystem<VelocitySystem>();

    world.Setup();
    world.Tick();

    auto* position0 = world.GetComponent<Position>(entity0);
    ASSERT_EQ(position0->x, 1.0f);
    ASSERT_EQ(position0->y, 2.0f);

    auto* position1 = world.GetComponent<Position>(entity1);
    ASSERT_EQ(position1->x, 6.0f);
    ASSERT_EQ(position1->y, 8.0f);

    world.Tick();

    ASSERT_EQ(position0->x, 1.0f);
    ASSERT_EQ(position0->y, 2.0f);

    ASSERT_EQ(position1->x, 7.0f);
    ASSERT_EQ(position1->y, 10.0f);
}

TEST(WorldTest, SystemSetupTest)
{
    Runtime::World world("SystemSetupText");

    auto entity0 = world.CreateEntity();
    world.AddComponent<Position>(entity0);

    world.MountSystem<PositionSetupSystem>(1.0f, 1.0f);

    world.Setup();

    auto* position0 = world.GetComponent<Position>(entity0);
    ASSERT_EQ(position0->x, 1.0f);
    ASSERT_EQ(position0->y, 1.0f);
}
