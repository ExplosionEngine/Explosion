//
// Created by johnk on 2023/9/20.
//

#include <gtest/gtest.h>

#include <Runtime/RuntimeModule.h>
#include <Core/Module.h>
using namespace Core;
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
        void Execute(Runtime::SystemCommands& commands) override
        {
            commands.StartQuery<Position, Velocity>().Each([](Position& position, Velocity& velocity) -> void {
                position.x += velocity.x;
                position.y += velocity.y;
                position.z += velocity.z;
            });
        }
    };

    struct WorldSetupSystem : public SetupSystem {
    public:
        void Execute(Runtime::SystemCommands& commands) override
        {
            commands.EmplaceGlobal<SavedEntities>(SavedEntities {
                {},
                commands.Create(),
                commands.Create()});

            const auto* savedEntities = commands.GetGlobal<SavedEntities>();
            commands.Emplace<Position>(savedEntities->testEntity0, Position {{}, 1.0f, 2.0f, 3.0f});
            commands.Emplace<Velocity>(savedEntities->testEntity0, Velocity {{}, 1.0f, 1.0f, 1.0f});
            commands.Emplace<Position>(savedEntities->testEntity1, Position {{}, 2.0f, 3.0f, 4.0f});
            commands.Emplace<Velocity>(savedEntities->testEntity1, Velocity {{}, 2.0f, 2.0f, 2.0f});
        }
    };

    World world;
    world.AddTickSystem<MotionSystem>();
    world.AddSetupSystem<WorldSetupSystem>();
    world.Setup();
    world.Tick();

    WorldTestHelper testHelper(world);
    SystemCommands commands = testHelper.HackCreateSystemCommands();

    ASSERT_TRUE(commands.HasGlobal<SavedEntities>());
    const auto* savedEntities = commands.GetGlobal<SavedEntities>();

    const auto* testPosition0 = commands.Get<Position>(savedEntities->testEntity0);
    const auto* testVelocity0 = commands.Get<Velocity>(savedEntities->testEntity0);
    ASSERT_TRUE(testPosition0 != nullptr);
    ASSERT_EQ(testPosition0->x, 2.0f);
    ASSERT_EQ(testPosition0->y, 3.0f);
    ASSERT_EQ(testPosition0->z, 4.0f);
    ASSERT_TRUE(testVelocity0 != nullptr);
    ASSERT_EQ(testVelocity0->x, 1.0f);
    ASSERT_EQ(testVelocity0->y, 1.0f);
    ASSERT_EQ(testVelocity0->z, 1.0f);

    const auto* testPosition1 = commands.Get<Position>(savedEntities->testEntity1);
    const auto* testVelocity1 = commands.Get<Velocity>(savedEntities->testEntity1);
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
        void Execute(Runtime::SystemCommands& commands) override
        {
            commands.EmplaceGlobal<TestGlobalComponent>(TestGlobalComponent {{}, 1, 2, 3});
            ASSERT_TRUE(commands.HasGlobal<TestGlobalComponent>());

            const auto* globalComponent = commands.GetGlobal<TestGlobalComponent>();
            ASSERT_EQ(globalComponent->a, 1);
            ASSERT_EQ(globalComponent->b, 2);
            ASSERT_EQ(globalComponent->c, 3);

            commands.PatchGlobal<TestGlobalComponent>([](TestGlobalComponent& component) -> void {
                component.a = 2;
                component.b = 3;
                component.c = 4;
            });
            globalComponent = commands.GetGlobal<TestGlobalComponent>();
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
        void Execute(Runtime::SystemCommands& commands) override
        {
            commands.EmplaceGlobal<Context>(Context {{}, false, false, false});
        }
    };

    struct System1 : public SetupSystem {
    public:
        void Execute(Runtime::SystemCommands& commands) override
        {
            ASSERT_TRUE(commands.HasGlobal<Context>());
            const auto* context = commands.GetGlobal<Context>();
            ASSERT_FALSE(context->system1Executed);
            ASSERT_FALSE(context->system2Executed);
            ASSERT_FALSE(context->system3Executed);

            commands.PatchGlobal<Context>([](Context& context) -> void {
                context.system1Executed = true;
            });
        }
    };

    struct System2 : public SetupSystem {
    public:
        void Execute(Runtime::SystemCommands& commands) override
        {
            ASSERT_TRUE(commands.HasGlobal<Context>());
            const auto* context = commands.GetGlobal<Context>();
            ASSERT_TRUE(context->system1Executed);
            ASSERT_FALSE(context->system2Executed);
            ASSERT_FALSE(context->system3Executed);

            commands.PatchGlobal<Context>([](Context& context) -> void {
                context.system2Executed = true;
            });
        }
    };

    struct System3 : public SetupSystem {
    public:
        void Execute(Runtime::SystemCommands& commands) override
        {
            ASSERT_TRUE(commands.HasGlobal<Context>());
            const auto* context = commands.GetGlobal<Context>();
            ASSERT_TRUE(context->system1Executed);
            ASSERT_TRUE(context->system2Executed);
            ASSERT_FALSE(context->system3Executed);

            commands.PatchGlobal<Context>([](Context& context) -> void {
                context.system3Executed = true;
            });
        }
    };

    World world;
    world.AddSetupSystem<WorldSetupSystem>();
    world.AddTickSystem<System1>();
    world.AddTickSystem<System2>()
        .ScheduleAfter<System1>();
    world.AddTickSystem<System3>()
        .ScheduleAfter<System2>();
    world.Setup();
    world.Tick();

    WorldTestHelper testHelper(world);
    SystemCommands commands = testHelper.HackCreateSystemCommands();

    ASSERT_TRUE(commands.HasGlobal<Context>());
    const auto* context = commands.GetGlobal<Context>();
    ASSERT_TRUE(context->system1Executed);
    ASSERT_TRUE(context->system2Executed);
    ASSERT_TRUE(context->system3Executed);

    world.Shutdown();
}

TEST(WorldTest, SystemEventTest)
{
    struct GlobalStatus : public GlobalComponent {
        bool globalComponentAdded;
        bool globalComponentUpdated;
        bool globalComponentRemoved;
        Entity testEntity;
        bool componentAdded;
        bool componentUpdated;
        bool componentRemoved;
    };

    struct EmptyGlobalComponent : public GlobalComponent {};
    struct EmptyComponent : public Component {};

    struct WorldSetupSystem : public SetupSystem {
        void Execute(Runtime::SystemCommands& commands) override
        {
            Entity testEntity = commands.Create();
            commands.EmplaceGlobal<GlobalStatus>(GlobalStatus {
                {},
                false,
                false,
                false,
                testEntity,
                false,
                false,
                false});

            commands.EmplaceGlobal<EmptyGlobalComponent>();
            commands.Emplace<EmptyComponent>(testEntity, EmptyComponent { {} });
        }
    };

    struct WorldTickSystem : public TickSystem {
        void Execute(Runtime::SystemCommands& commands) override
        {
            const auto* globalState = commands.GetGlobal<GlobalStatus>();
            if (globalState->globalComponentUpdated) {
                ASSERT_TRUE(globalState->globalComponentAdded);
            }
            if (globalState->globalComponentRemoved) {
                ASSERT_TRUE(globalState->globalComponentAdded);
                ASSERT_TRUE(globalState->globalComponentUpdated);
            }
            if (globalState->componentUpdated) {
                ASSERT_TRUE(globalState->componentAdded);
            }
            if (globalState->componentRemoved) {
                ASSERT_TRUE(globalState->componentAdded);
                ASSERT_TRUE(globalState->componentUpdated);
            }

            if (globalState->globalComponentAdded && !globalState->globalComponentUpdated) {
                commands.UpdatedGlobal<EmptyGlobalComponent>();
            }
            if (globalState->globalComponentUpdated && !globalState->globalComponentRemoved) {
                commands.RemoveGlobal<EmptyGlobalComponent>();
            }
            if (globalState->componentAdded && !globalState->componentUpdated) {
                commands.Updated<EmptyComponent>(globalState->testEntity);
            }
            if (globalState->componentUpdated && !globalState->componentRemoved) {
                commands.Remove<EmptyComponent>(globalState->testEntity);
            }
        }
    };

    struct OnGlobalComponentAddedSystem : public EventSystem {
        void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
        {
            auto* globalStatus = commands.GetGlobal<GlobalStatus>();
            globalStatus->globalComponentAdded = true;
            commands.UpdatedGlobal<GlobalStatus>();
        }
    };

    struct OnGlobalComponentUpdatedSystem : public EventSystem {
        void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
        {
            auto* globalStatus = commands.GetGlobal<GlobalStatus>();
            globalStatus->globalComponentUpdated = true;
            commands.UpdatedGlobal<GlobalStatus>();
        }
    };

    struct OnGlobalComponentRemoveSystem : public EventSystem {
        void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
        {
            auto* globalStatus = commands.GetGlobal<GlobalStatus>();
            globalStatus->globalComponentRemoved = true;
            commands.UpdatedGlobal<GlobalStatus>();
        }
    };

    struct OnComponentAddedSystem : public EventSystem {
        void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
        {
            SystemEventDecoder<ComponentAdded<EmptyComponent>> decoder(eventRef);
            Entity entity = commands.GetGlobal<GlobalStatus>()->testEntity;
            ASSERT_EQ(entity, decoder.Get().entity);

            auto* globalStatus = commands.GetGlobal<GlobalStatus>();
            globalStatus->componentAdded = true;
            commands.UpdatedGlobal<GlobalStatus>();
        }
    };

    struct OnComponentUpdatedSystem : public EventSystem {
        void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
        {
            SystemEventDecoder<ComponentUpdated<EmptyComponent>> decoder(eventRef);
            Entity entity = commands.GetGlobal<GlobalStatus>()->testEntity;
            ASSERT_EQ(entity, decoder.Get().entity);

            auto* globalStatus = commands.GetGlobal<GlobalStatus>();
            globalStatus->componentUpdated = true;
            commands.UpdatedGlobal<GlobalStatus>();
        }
    };

    struct OnComponentRemovedSystem : public EventSystem {
        void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
        {
            SystemEventDecoder<ComponentRemoved<EmptyComponent>> decoder(eventRef);
            Entity entity = commands.GetGlobal<GlobalStatus>()->testEntity;
            ASSERT_EQ(entity, decoder.Get().entity);

            auto* globalStatus = commands.GetGlobal<GlobalStatus>();
            globalStatus->componentRemoved = true;
            commands.UpdatedGlobal<GlobalStatus>();
        }
    };

    World world;
    world.AddSetupSystem<WorldSetupSystem>();
    world.AddTickSystem<WorldTickSystem>();
    world.Event<GlobalComponentAdded<EmptyGlobalComponent>>().Connect<OnGlobalComponentAddedSystem>();
    world.Event<GlobalComponentUpdated<EmptyGlobalComponent>>().Connect<OnGlobalComponentUpdatedSystem>();
    world.Event<GlobalComponentRemoved<EmptyGlobalComponent>>().Connect<OnGlobalComponentRemoveSystem>();
    world.Event<ComponentAdded<EmptyComponent>>().Connect<OnComponentAddedSystem>();
    world.Event<ComponentUpdated<EmptyComponent>>().Connect<OnComponentUpdatedSystem>();
    world.Event<ComponentRemoved<EmptyComponent>>().Connect<OnComponentRemovedSystem>();

    world.Setup();
    world.Tick();
    world.Tick();
    world.Tick();
    world.Shutdown();
}

TEST(WorldTest, CustomSystemEventTest)
{
    struct CustomEvent {
        int value;
    };

    struct WorldSetupSystem : public SetupSystem {
        void Execute(Runtime::SystemCommands& commands) override
        {
            commands.Broadcast(CustomEvent { 1 });
        }
    };

    struct CustomEventSystem : public EventSystem {
        void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
        {
            SystemEventDecoder<CustomEvent> decoder(eventRef);
            ASSERT_EQ(decoder.Get().value, 1);
        }
    };

    World world;
    world.AddSetupSystem<WorldSetupSystem>();
    world.Event<CustomEvent>().Connect<CustomEventSystem>();

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
