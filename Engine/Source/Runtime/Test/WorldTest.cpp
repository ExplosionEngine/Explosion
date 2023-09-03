//
// Created by johnk on 2022/8/15.
//

#include <gtest/gtest.h>

#include <Runtime/World.h>
#include <WorldTest.h>

TEST(WorldTest, ComponentBasicTest)
{
    Runtime::World world("TestWorld");

    world.RegisterComponentType<PositionComponent>();

    auto entity = world.CreateEntity();
    world.AddComponent<PositionComponent>(entity, 1.0f, 2.0f);

    auto* position = world.GetComponent<PositionComponent>(entity);
    ASSERT_NE(position, nullptr);
    ASSERT_EQ(position->x, 1.0f);
    ASSERT_EQ(position->y, 2.0f);

    world.RemoveComponent<PositionComponent>(entity);
    position = world.GetComponent<PositionComponent>(entity);
    ASSERT_EQ(position, nullptr);
}

TEST(WorldTest, SystemBasicTest)
{
    Runtime::World world("SystemBasicTestWorld");

    world.RegisterComponentType<PositionComponent>();
    world.RegisterComponentType<VelocityComponent>();

    auto entity0 = world.CreateEntity();
    world.AddComponent<PositionComponent>(entity0, 1.0f, 2.0f);
    world.AddComponent<VelocityComponent>(entity0, 0.0f, 0.0f);

    auto entity1 = world.CreateEntity();
    world.AddComponent<PositionComponent>(entity1, 5.0f, 6.0f);
    world.AddComponent<VelocityComponent>(entity1, 1.0f, 2.0f);

    world.MountSystem<VelocitySystem>();

    world.Setup();
    world.Tick();

    auto* position0 = world.GetComponent<PositionComponent>(entity0);
    ASSERT_EQ(position0->x, 1.0f);
    ASSERT_EQ(position0->y, 2.0f);

    auto* position1 = world.GetComponent<PositionComponent>(entity1);
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
    Runtime::World world("SystemSetupTest");

    world.RegisterComponentType<PositionComponent>();

    auto entity0 = world.CreateEntity();
    world.AddComponent<PositionComponent>(entity0);

    world.MountSystem<PositionSetupSystem>(1.0f, 1.0f);

    world.Setup();

    auto* position0 = world.GetComponent<PositionComponent>(entity0);
    ASSERT_EQ(position0->x, 1.0f);
    ASSERT_EQ(position0->y, 1.0f);
}

TEST(WorldTest, EngineComponentTypesTest)
{
    Common::UniqueRef<Runtime::World> world = Common::MakeUnique<Runtime::World>("EngineComponentTypesTest");
    world->RegisterEngineComponentTypes();

    auto entity0 = world->CreateEntity();
    auto& entityInfoComponent = world->AddComponent<Runtime::EntityInfoComponent>(entity0);
    entityInfoComponent.SetName("entity0");

    ASSERT_EQ(world->GetComponent<Runtime::EntityInfoComponent>(entity0)->GetName(), "entity0");
}

TEST(WorldTest, ComponentLifecycleFuncTest)
{
    Runtime::World world("ComponentLifecycleTest");
    world.RegisterComponentType<LifecycleTestComponent>();

    uint32_t onConstructCallTime0 = 0;
    uint32_t onDestroyCallTime0 = 0;
    auto entity0 = world.CreateEntity();
    world.AddComponent<LifecycleTestComponent>(
        entity0,
        [&onConstructCallTime0]() -> void { onConstructCallTime0++; },
        [&onDestroyCallTime0]() -> void { onDestroyCallTime0++; });

    ASSERT_EQ(onConstructCallTime0, 1);
    ASSERT_EQ(onDestroyCallTime0, 0);

    uint32_t onConstructCallTime1 = 0;
    uint32_t onDestroyCallTime1 = 0;
    auto entity1 = world.CreateEntity();
    world.AddComponent<LifecycleTestComponent>(
        entity1,
        [&onConstructCallTime1]() -> void { onConstructCallTime1++; },
        [&onDestroyCallTime1]() -> void { onDestroyCallTime1++; });

    ASSERT_EQ(onConstructCallTime0, 1);
    ASSERT_EQ(onDestroyCallTime0, 0);
    ASSERT_EQ(onConstructCallTime1, 1);
    ASSERT_EQ(onDestroyCallTime1, 0);

    world.DestroyEntity(entity0);
    ASSERT_EQ(onConstructCallTime0, 1);
    ASSERT_EQ(onDestroyCallTime0, 1);
    ASSERT_EQ(onConstructCallTime1, 1);
    ASSERT_EQ(onDestroyCallTime1, 0);
}
