//
// Created by johnk on 2023/10/3.
//

#pragma once

#include <gtest/gtest.h>

#include <Runtime/RuntimeModule.h>
using namespace Core;
using namespace Runtime;

struct BasicTest_Position : public Component {
    float x;
    float y;
    float z;
};

struct BasicTest_Velocity : public Component {
    float x;
    float y;
    float z;
};

struct BasicTest_GlobalStatus : public GlobalComponent {
    Entity testEntity0 = entityNull;
    Entity testEntity1 = entityNull;
};

struct BasicTest_MotionSystem : public TickSystem {
public:
    void Execute(Runtime::SystemCommands& commands) override
    {
        commands.StartQuery<BasicTest_Position, BasicTest_Velocity>().Each([](BasicTest_Position& position, BasicTest_Velocity& velocity) -> void {
            position.x += velocity.x;
            position.y += velocity.y;
            position.z += velocity.z;
        });
    }
};

struct BasicTest_WorldSetupSystem : public SetupSystem {
public:
    void Execute(Runtime::SystemCommands& commands) override
    {
        commands.EmplaceGlobal<BasicTest_GlobalStatus>(BasicTest_GlobalStatus {
            {},
            commands.Create(),
            commands.Create()
        });

        const auto* savedEntities = commands.GetGlobal<BasicTest_GlobalStatus>();
        commands.Emplace<BasicTest_Position>(savedEntities->testEntity0, BasicTest_Position {{}, 1.0f, 2.0f, 3.0f});
        commands.Emplace<BasicTest_Velocity>(savedEntities->testEntity0, BasicTest_Velocity {{}, 1.0f, 1.0f, 1.0f});
        commands.Emplace<BasicTest_Position>(savedEntities->testEntity1, BasicTest_Position {{}, 2.0f, 3.0f, 4.0f});
        commands.Emplace<BasicTest_Velocity>(savedEntities->testEntity1, BasicTest_Velocity {{}, 2.0f, 2.0f, 2.0f});
    }
};

struct GlobalComponentTest_TestGlobalComponent : public GlobalComponent {
    int32_t a;
    int32_t b;
    int32_t c;
};

struct GlobalComponentTest_WorldSetupSystem : public SetupSystem {
public:
    void Execute(Runtime::SystemCommands& commands) override
    {
        commands.EmplaceGlobal<GlobalComponentTest_TestGlobalComponent>(GlobalComponentTest_TestGlobalComponent {{}, 1, 2, 3});
        ASSERT_TRUE(commands.HasGlobal<GlobalComponentTest_TestGlobalComponent>());

        const auto* globalComponent = commands.GetGlobal<GlobalComponentTest_TestGlobalComponent>();
        ASSERT_EQ(globalComponent->a, 1);
        ASSERT_EQ(globalComponent->b, 2);
        ASSERT_EQ(globalComponent->c, 3);

        commands.PatchGlobal<GlobalComponentTest_TestGlobalComponent>([](GlobalComponentTest_TestGlobalComponent& component) -> void {
            component.a = 2;
            component.b = 3;
            component.c = 4;
        });
        globalComponent = commands.GetGlobal<GlobalComponentTest_TestGlobalComponent>();
        ASSERT_EQ(globalComponent->a, 2);
        ASSERT_EQ(globalComponent->b, 3);
        ASSERT_EQ(globalComponent->c, 4);
    }
};

struct SystemScheduleTest_Context : public GlobalComponent {
    bool system1Executed;
    bool system2Executed;
    bool system3Executed;
};

struct SystemScheduleTest_WorldSetupSystem : public SetupSystem {
public:
    void Execute(Runtime::SystemCommands& commands) override
    {
        commands.EmplaceGlobal<SystemScheduleTest_Context>(SystemScheduleTest_Context {{}, false, false, false});
    }
};

struct SystemScheduleTest_System1 : public SetupSystem {
public:
    void Execute(Runtime::SystemCommands& commands) override
    {
        ASSERT_TRUE(commands.HasGlobal<SystemScheduleTest_Context>());
        const auto* context = commands.GetGlobal<SystemScheduleTest_Context>();
        ASSERT_FALSE(context->system1Executed);
        ASSERT_FALSE(context->system2Executed);
        ASSERT_FALSE(context->system3Executed);

        commands.PatchGlobal<SystemScheduleTest_Context>([](SystemScheduleTest_Context& context) -> void {
            context.system1Executed = true;
        });
    }
};

struct SystemScheduleTest_System2 : public SetupSystem {
public:
    void Execute(Runtime::SystemCommands& commands) override
    {
        ASSERT_TRUE(commands.HasGlobal<SystemScheduleTest_Context>());
        const auto* context = commands.GetGlobal<SystemScheduleTest_Context>();
        ASSERT_TRUE(context->system1Executed);
        ASSERT_FALSE(context->system2Executed);
        ASSERT_FALSE(context->system3Executed);

        commands.PatchGlobal<SystemScheduleTest_Context>([](SystemScheduleTest_Context& context) -> void {
            context.system2Executed = true;
        });
    }
};

struct SystemScheduleTest_System3 : public SetupSystem {
public:
    void Execute(Runtime::SystemCommands& commands) override
    {
        ASSERT_TRUE(commands.HasGlobal<SystemScheduleTest_Context>());
        const auto* context = commands.GetGlobal<SystemScheduleTest_Context>();
        ASSERT_TRUE(context->system1Executed);
        ASSERT_TRUE(context->system2Executed);
        ASSERT_FALSE(context->system3Executed);

        commands.PatchGlobal<SystemScheduleTest_Context>([](SystemScheduleTest_Context& context) -> void {
            context.system3Executed = true;
        });
    }
};

struct SystemEventTest_GlobalStatus : public GlobalComponent {
    bool globalComponentAdded;
    bool globalComponentUpdated;
    bool globalComponentRemoved;
    Entity testEntity;
    bool componentAdded;
    bool componentUpdated;
    bool componentRemoved;
};

struct SystemEventTest_EmptyGlobalComponent : public GlobalComponent {};
struct SystemEventTest_EmptyComponent : public Component {};

struct SystemEventTest_WorldSetupSystem : public SetupSystem {
    void Execute(Runtime::SystemCommands& commands) override
    {
        Entity testEntity = commands.Create();
        commands.EmplaceGlobal<SystemEventTest_GlobalStatus>(SystemEventTest_GlobalStatus {
            {},
            false,
            false,
            false,
            testEntity,
            false,
            false,
            false});

        commands.EmplaceGlobal<SystemEventTest_EmptyGlobalComponent>();
        commands.Emplace<SystemEventTest_EmptyComponent>(testEntity, SystemEventTest_EmptyComponent { {} });
    }
};

struct SystemEventTest_WorldTickSystem : public TickSystem {
    void Execute(Runtime::SystemCommands& commands) override
    {
        const auto* globalState = commands.GetGlobal<SystemEventTest_GlobalStatus>();
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
            commands.UpdatedGlobal<SystemEventTest_EmptyGlobalComponent>();
        }
        if (globalState->globalComponentUpdated && !globalState->globalComponentRemoved) {
            commands.RemoveGlobal<SystemEventTest_EmptyGlobalComponent>();
        }
        if (globalState->componentAdded && !globalState->componentUpdated) {
            commands.Updated<SystemEventTest_EmptyComponent>(globalState->testEntity);
        }
        if (globalState->componentUpdated && !globalState->componentRemoved) {
            commands.Remove<SystemEventTest_EmptyComponent>(globalState->testEntity);
        }
    }
};

struct SystemEventTest_OnGlobalComponentAddedSystem : public EventSystem {
    void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
    {
        auto* globalStatus = commands.GetGlobal<SystemEventTest_GlobalStatus>();
        globalStatus->globalComponentAdded = true;
        commands.UpdatedGlobal<SystemEventTest_GlobalStatus>();
    }
};

struct SystemEventTest_OnGlobalComponentUpdatedSystem : public EventSystem {
    void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
    {
        auto* globalStatus = commands.GetGlobal<SystemEventTest_GlobalStatus>();
        globalStatus->globalComponentUpdated = true;
        commands.UpdatedGlobal<SystemEventTest_GlobalStatus>();
    }
};

struct SystemEventTest_OnGlobalComponentRemoveSystem : public EventSystem {
    void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
    {
        auto* globalStatus = commands.GetGlobal<SystemEventTest_GlobalStatus>();
        globalStatus->globalComponentRemoved = true;
        commands.UpdatedGlobal<SystemEventTest_GlobalStatus>();
    }
};

struct SystemEventTest_OnComponentAddedSystem : public EventSystem {
    void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
    {
        SystemEventDecoder<ComponentAdded<SystemEventTest_EmptyComponent>> decoder(eventRef);
        Entity entity = commands.GetGlobal<SystemEventTest_GlobalStatus>()->testEntity;
        ASSERT_EQ(entity, decoder.Get().entity);

        auto* globalStatus = commands.GetGlobal<SystemEventTest_GlobalStatus>();
        globalStatus->componentAdded = true;
        commands.UpdatedGlobal<SystemEventTest_GlobalStatus>();
    }
};

struct SystemEventTest_OnComponentUpdatedSystem : public EventSystem {
    void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
    {
        SystemEventDecoder<ComponentUpdated<SystemEventTest_EmptyComponent>> decoder(eventRef);
        Entity entity = commands.GetGlobal<SystemEventTest_GlobalStatus>()->testEntity;
        ASSERT_EQ(entity, decoder.Get().entity);

        auto* globalStatus = commands.GetGlobal<SystemEventTest_GlobalStatus>();
        globalStatus->componentUpdated = true;
        commands.UpdatedGlobal<SystemEventTest_GlobalStatus>();
    }
};

struct SystemEventTest_OnComponentRemovedSystem : public EventSystem {
    void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
    {
        SystemEventDecoder<ComponentRemoved<SystemEventTest_EmptyComponent>> decoder(eventRef);
        Entity entity = commands.GetGlobal<SystemEventTest_GlobalStatus>()->testEntity;
        ASSERT_EQ(entity, decoder.Get().entity);

        auto* globalStatus = commands.GetGlobal<SystemEventTest_GlobalStatus>();
        globalStatus->componentRemoved = true;
        commands.UpdatedGlobal<SystemEventTest_GlobalStatus>();
    }
};

struct CustomSystemEventTest_CustomEvent {
    int value;
};

struct CustomSystemEventTest_WorldSetupSystem : public SetupSystem {
    void Execute(Runtime::SystemCommands& commands) override
    {
        commands.Broadcast(CustomSystemEventTest_CustomEvent { 1 });
    }
};

struct CustomSystemEventTest_CustomEventSystem : public EventSystem {
    void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
    {
        SystemEventDecoder<CustomSystemEventTest_CustomEvent> decoder(eventRef);
        ASSERT_EQ(decoder.Get().value, 1);
    }
};
