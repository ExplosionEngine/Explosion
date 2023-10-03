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
    world.Tick();

    WorldTestHelper testHelper(world);
    SystemCommands commands = testHelper.HackCreateSystemCommands();

    ASSERT_TRUE(commands.HasGlobal<BasicTest_GlobalStatus>());
    const auto* savedEntities = commands.GetGlobal<BasicTest_GlobalStatus>();

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

TEST(WorldTest, GlobalComponentTest)
{
    World world;
    world.AddSetupSystem<GlobalComponentTest_WorldSetupSystem>();
    world.Setup();
    world.Shutdown();
}

TEST(WorldTest, SystemScheduleTest)
{
    World world;
    world.AddSetupSystem<SystemScheduleTest_WorldSetupSystem>();
    world.AddTickSystem<SystemScheduleTest_System1>();
    world.AddTickSystem<SystemScheduleTest_System2>()
        .ScheduleAfter<SystemScheduleTest_System1>();
    world.AddTickSystem<SystemScheduleTest_System3>()
        .ScheduleAfter<SystemScheduleTest_System2>();
    world.Setup();
    world.Tick();

    WorldTestHelper testHelper(world);
    SystemCommands commands = testHelper.HackCreateSystemCommands();

    ASSERT_TRUE(commands.HasGlobal<SystemScheduleTest_Context>());
    const auto* context = commands.GetGlobal<SystemScheduleTest_Context>();
    ASSERT_TRUE(context->system1Executed);
    ASSERT_TRUE(context->system2Executed);
    ASSERT_TRUE(context->system3Executed);

    world.Shutdown();
}

TEST(WorldTest, SystemEventTest)
{
    World world;
    world.AddSetupSystem<SystemEventTest_WorldSetupSystem>();
    world.AddTickSystem<SystemEventTest_WorldTickSystem>();
    world.Event<GlobalComponentAdded<SystemEventTest_EmptyGlobalComponent>>().Connect<SystemEventTest_OnGlobalComponentAddedSystem>();
    world.Event<GlobalComponentUpdated<SystemEventTest_EmptyGlobalComponent>>().Connect<SystemEventTest_OnGlobalComponentUpdatedSystem>();
    world.Event<GlobalComponentRemoved<SystemEventTest_EmptyGlobalComponent>>().Connect<SystemEventTest_OnGlobalComponentRemoveSystem>();
    world.Event<ComponentAdded<SystemEventTest_EmptyComponent>>().Connect<SystemEventTest_OnComponentAddedSystem>();
    world.Event<ComponentUpdated<SystemEventTest_EmptyComponent>>().Connect<SystemEventTest_OnComponentUpdatedSystem>();
    world.Event<ComponentRemoved<SystemEventTest_EmptyComponent>>().Connect<SystemEventTest_OnComponentRemovedSystem>();

    world.Setup();
    world.Tick();
    world.Tick();
    world.Tick();
    world.Shutdown();
}

TEST(WorldTest, CustomSystemEventTest)
{
    World world;
    world.AddSetupSystem<CustomSystemEventTest_WorldSetupSystem>();
    world.Event<CustomSystemEventTest_CustomEvent>().Connect<CustomSystemEventTest_CustomEventSystem>();

    world.Setup();
    world.Tick();
    world.Shutdown();
}

TEST(WorldTest, ModuleTest)
{
    auto* module = ModuleManager::Get().FindOrLoadTyped<RuntimeModule>("Runtime");
    auto* world = module->CreateWorld();
    module->DestroyWorld(world);
}
