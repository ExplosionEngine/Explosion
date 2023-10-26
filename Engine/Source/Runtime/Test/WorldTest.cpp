//
// Created by johnk on 2023/9/20.
//

#include <WorldTest.h>

TEST(WorldTest, BasicTest)
{
    World world;
    world.AddTickSystem<BasicTest_MotionSystem>();
    world.AddSetupSystem<BasicTest_WorldSetupSystem>();
    world.Setup();
    world.Tick(0.01f);

    SystemCommands commands(world);

    ASSERT_TRUE(commands.HasState<BasicTest_GlobalState>());
    const auto* savedEntities = commands.GetState<BasicTest_GlobalState>();

    const auto* testPosition0 = commands.Get<BasicTest_Position>(savedEntities->testEntity0);
    const auto* testVelocity0 = commands.Get<BasicTest_Velocity>(savedEntities->testEntity0);
    ASSERT_TRUE(testPosition0 != nullptr);
    ASSERT_EQ(testPosition0->x, 2.0f);
    ASSERT_EQ(testPosition0->y, 3.0f);
    ASSERT_EQ(testPosition0->z, 4.0f);
    ASSERT_TRUE(testVelocity0 != nullptr);
    ASSERT_EQ(testVelocity0->x, 1.0f);
    ASSERT_EQ(testVelocity0->y, 1.0f);
    ASSERT_EQ(testVelocity0->z, 1.0f);

    const auto* testPosition1 = commands.Get<BasicTest_Position>(savedEntities->testEntity1);
    const auto* testVelocity1 = commands.Get<BasicTest_Velocity>(savedEntities->testEntity1);
    ASSERT_TRUE(testPosition1 != nullptr);
    ASSERT_EQ(testPosition1->x, 4.0f);
    ASSERT_EQ(testPosition1->y, 5.0f);
    ASSERT_EQ(testPosition1->z, 6.0f);
    ASSERT_TRUE(testVelocity1 != nullptr);
    ASSERT_EQ(testVelocity1->x, 2.0f);
    ASSERT_EQ(testVelocity1->y, 2.0f);
    ASSERT_EQ(testVelocity1->z, 2.0f);

    world.Shutdown();
}

TEST(WorldTest, StateTest)
{
    World world;
    world.AddSetupSystem<StateTest_WorldSetupSystem>();
    world.Setup();
    world.Shutdown();
}

TEST(WorldTest, SystemScheduleTest)
{
    World world;
    world.AddSetupSystem<SystemScheduleTest_WorldSetupSystem>();
    world.AddTickSystem<SystemScheduleTest_System1>();
    world.AddTickSystem<SystemScheduleTest_System2>();
    world.AddTickSystem<SystemScheduleTest_System3>();
    world.Setup();
    world.Tick(0.01f);

    SystemCommands commands(world);

    ASSERT_TRUE(commands.HasState<SystemScheduleTest_Context>());
    const auto* context = commands.GetState<SystemScheduleTest_Context>();
    ASSERT_TRUE(context->system1Executed);
    ASSERT_TRUE(context->system2Executed);
    ASSERT_TRUE(context->system3Executed);

    world.Shutdown();
}

TEST(WorldTest, EventTest)
{
    World world;
    world.AddSetupSystem<EventTest_WorldSetupSystem>();
    world.AddTickSystem<EventTest_WorldTickSystem>();
    world.AddEventSystem<EventTest_EmptyState::Added, EventTest_OnStateAddedSystem>();
    world.AddEventSystem<EventTest_EmptyState::Updated, EventTest_OnStateUpdatedSystem>();
    world.AddEventSystem<EventTest_EmptyState::Removed, EventTest_OnStateRemoveSystem>();
    world.AddEventSystem<EventTest_EmptyComponent::Added, EventTest_OnComponentAddedSystem>();
    world.AddEventSystem<EventTest_EmptyComponent::Updated, EventTest_OnComponentUpdatedSystem>();
    world.AddEventSystem<EventTest_EmptyComponent::Removed, EventTest_OnComponentRemovedSystem>();

    world.Setup();
    world.Tick(0.01f);
    world.Tick(0.01f);
    world.Tick(0.01f);
    world.Shutdown();
}

TEST(WorldTest, CustomEventTest)
{
    World world;
    world.AddSetupSystem<CustomEventTest_WorldSetupSystem>();
    world.AddEventSystem<CustomEventTest_CustomEvent, CustomEventTest_CustomEventSystem>();

    world.Setup();
    world.Tick(0.01f);
    world.Shutdown();
}

TEST(WorldTest, ModuleTest)
{
    auto* module = ModuleManager::Get().FindOrLoadTyped<RuntimeModule>("Runtime");
    auto* world = module->CreateWorld();
    module->DestroyWorld(world);
}
