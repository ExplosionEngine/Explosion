//
// Created by johnk on 2023/9/20.
//

#include <gtest/gtest.h>

#include <Runtime/World.h>
using namespace Runtime;

TEST(WorldTest, BasicTest)
{
    struct Position : public Component {
        float x;
        float y;
        float z;
    };

    struct Velocity : public Component {
        float x;
        float y;
        float z;
    };

    struct SavedEntities : public GlobalComponent {
        Entity testEntity0 = entityNull;
        Entity testEntity1 = entityNull;
    };

    struct MotionSystem : public TickSystem {
    public:
        void Execute(Runtime::SystemCommands& systemCommands) override
        {
            systemCommands.NewQuery<Position, Velocity>().Each([](Position& position, Velocity& velocity) -> void {
                position.x += velocity.x;
                position.y += velocity.y;
                position.z += velocity.z;
            });
        }
    };

    struct WorldSetupSystem : public SetupSystem {
    public:
        void Execute(Runtime::SystemCommands& systemCommands) override
        {
            systemCommands.AddGlobalComponent<SavedEntities>(SavedEntities {
                {},
                systemCommands.AddEntity(),
                systemCommands.AddEntity()
            });

            const auto* savedEntities = systemCommands.GetGlobalComponent<SavedEntities>();
            systemCommands.AddComponent<Position>(savedEntities->testEntity0, Position { {}, 1.0f, 2.0f, 3.0f });
            systemCommands.AddComponent<Velocity>(savedEntities->testEntity0, Velocity { {}, 1.0f, 1.0f, 1.0f });
            systemCommands.AddComponent<Position>(savedEntities->testEntity1, Position { {}, 2.0f, 3.0f, 4.0f });
            systemCommands.AddComponent<Velocity>(savedEntities->testEntity1, Velocity { {}, 2.0f, 2.0f, 2.0f });
        }
    };

    World world;
    world.AddTickSystem<MotionSystem>();
    world.AddSetupSystem<WorldSetupSystem>();
    world.Setup();
    world.Tick();

    WorldTestHelper testHelper(world);
    SystemCommands systemCommands = testHelper.HackCreateSystemCommands();

    ASSERT_TRUE(systemCommands.HasGlobalComponent<SavedEntities>());
    const auto* savedEntities = systemCommands.GetGlobalComponent<SavedEntities>();

    const auto* testPosition0 = systemCommands.GetComponent<Position>(savedEntities->testEntity0);
    const auto* testVelocity0 = systemCommands.GetComponent<Velocity>(savedEntities->testEntity0);
    ASSERT_TRUE(testPosition0 != nullptr);
    ASSERT_EQ(testPosition0->x, 2.0f);
    ASSERT_EQ(testPosition0->y, 3.0f);
    ASSERT_EQ(testPosition0->z, 4.0f);
    ASSERT_TRUE(testVelocity0 != nullptr);
    ASSERT_EQ(testVelocity0->x, 1.0f);
    ASSERT_EQ(testVelocity0->y, 1.0f);
    ASSERT_EQ(testVelocity0->z, 1.0f);

    const auto* testPosition1 = systemCommands.GetComponent<Position>(savedEntities->testEntity1);
    const auto* testVelocity1 = systemCommands.GetComponent<Velocity>(savedEntities->testEntity1);
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
    struct TestGlobalComponent : public GlobalComponent {
        int32_t a;
        int32_t b;
        int32_t c;
    };

    struct WorldSetupSystem : public SetupSystem {
    public:
        void Execute(Runtime::SystemCommands& systemCommands) override
        {
            systemCommands.AddGlobalComponent<TestGlobalComponent>(TestGlobalComponent { {}, 1, 2, 3 });
            ASSERT_TRUE(systemCommands.HasGlobalComponent<TestGlobalComponent>());

            const auto* globalComponent = systemCommands.GetGlobalComponent<TestGlobalComponent>();
            ASSERT_EQ(globalComponent->a, 1);
            ASSERT_EQ(globalComponent->b, 2);
            ASSERT_EQ(globalComponent->c, 3);

            systemCommands.PatchGlobalComponent<TestGlobalComponent>([](TestGlobalComponent& component) -> void {
                component.a = 2;
                component.b = 3;
                component.c = 4;
            });
            globalComponent = systemCommands.GetGlobalComponent<TestGlobalComponent>();
            ASSERT_EQ(globalComponent->a, 2);
            ASSERT_EQ(globalComponent->b, 3);
            ASSERT_EQ(globalComponent->c, 4);
        }
    };

    World world;
    world.AddSetupSystem<WorldSetupSystem>();
    world.Setup();
    world.Shutdown();
}

TEST(WorldTest, SystemScheduleTest)
{
    struct Context : public GlobalComponent {
        bool system1Executed;
        bool system2Executed;
        bool system3Executed;
    };

    struct WorldSetupSystem : public SetupSystem {
    public:
        void Execute(Runtime::SystemCommands& systemCommands) override
        {
            systemCommands.AddGlobalComponent<Context>(Context { {}, false, false, false });
        }
    };

    struct System1 : public SetupSystem {
    public:
        void Execute(Runtime::SystemCommands& systemCommands) override
        {
            ASSERT_TRUE(systemCommands.HasGlobalComponent<Context>());
            const auto* context = systemCommands.GetGlobalComponent<Context>();
            ASSERT_FALSE(context->system1Executed);
            ASSERT_FALSE(context->system2Executed);
            ASSERT_FALSE(context->system3Executed);

            systemCommands.PatchGlobalComponent<Context>([](Context& context) -> void {
                context.system1Executed = true;
            });
        }
    };

    struct System2 : public SetupSystem {
    public:
        void Execute(Runtime::SystemCommands& systemCommands) override
        {
            ASSERT_TRUE(systemCommands.HasGlobalComponent<Context>());
            const auto* context = systemCommands.GetGlobalComponent<Context>();
            ASSERT_TRUE(context->system1Executed);
            ASSERT_FALSE(context->system2Executed);
            ASSERT_FALSE(context->system3Executed);

            systemCommands.PatchGlobalComponent<Context>([](Context& context) -> void {
                context.system2Executed = true;
            });
        }
    };

    struct System3 : public SetupSystem {
    public:
        void Execute(Runtime::SystemCommands& systemCommands) override
        {
            ASSERT_TRUE(systemCommands.HasGlobalComponent<Context>());
            const auto* context = systemCommands.GetGlobalComponent<Context>();
            ASSERT_TRUE(context->system1Executed);
            ASSERT_TRUE(context->system2Executed);
            ASSERT_FALSE(context->system3Executed);

            systemCommands.PatchGlobalComponent<Context>([](Context& context) -> void {
                context.system3Executed = true;
            });
        }
    };

    World world;
    world.AddSetupSystem<WorldSetupSystem>();
    world
        .AddTickSystem<System1>()
        .AddTickSystem<System2>(
            SystemSchedule()
                .ScheduleAfter<System1>()
        )
        .AddTickSystem<System3>(
            SystemSchedule()
                .ScheduleAfter<System2>()
        );
    world.Setup();
    world.Tick();

    WorldTestHelper testHelper(world);
    SystemCommands systemCommands = testHelper.HackCreateSystemCommands();

    ASSERT_TRUE(systemCommands.HasGlobalComponent<Context>());
    const auto* context = systemCommands.GetGlobalComponent<Context>();
    ASSERT_TRUE(context->system1Executed);
    ASSERT_TRUE(context->system2Executed);
    ASSERT_TRUE(context->system3Executed);

    world.Shutdown();
}

TEST(WorldTest, SystemEventTest)
{

}

TEST(WorldTest, CustomSystemEventTest)
{
    // TODO
}
