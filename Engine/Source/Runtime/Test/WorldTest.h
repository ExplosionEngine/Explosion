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

struct BasicTest_GlobalStatus : public State {
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
        commands.EmplaceState<BasicTest_GlobalStatus>(BasicTest_GlobalStatus {
            {},
            commands.Create(),
            commands.Create()
        });

        const auto* savedEntities = commands.GetState<BasicTest_GlobalStatus>();
        commands.Emplace<BasicTest_Position>(savedEntities->testEntity0, BasicTest_Position {{}, 1.0f, 2.0f, 3.0f});
        commands.Emplace<BasicTest_Velocity>(savedEntities->testEntity0, BasicTest_Velocity {{}, 1.0f, 1.0f, 1.0f});
        commands.Emplace<BasicTest_Position>(savedEntities->testEntity1, BasicTest_Position {{}, 2.0f, 3.0f, 4.0f});
        commands.Emplace<BasicTest_Velocity>(savedEntities->testEntity1, BasicTest_Velocity {{}, 2.0f, 2.0f, 2.0f});
    }
};

struct StateTest_TestState : public State {
    int32_t a;
    int32_t b;
    int32_t c;
};

struct StateTest_WorldSetupSystem : public SetupSystem {
public:
    void Execute(Runtime::SystemCommands& commands) override
    {
        commands.EmplaceState<StateTest_TestState>(
            StateTest_TestState { {}, 1, 2, 3 });
        ASSERT_TRUE(commands.HasState<StateTest_TestState>());

        const auto* state = commands.GetState<StateTest_TestState>();
        ASSERT_EQ(state->a, 1);
        ASSERT_EQ(state->b, 2);
        ASSERT_EQ(state->c, 3);

        commands.PatchState<StateTest_TestState>(
            [](StateTest_TestState& component) -> void
            {
                component.a = 2;
                component.b = 3;
                component.c = 4;
            });
        state = commands.GetState<StateTest_TestState>();
        ASSERT_EQ(state->a, 2);
        ASSERT_EQ(state->b, 3);
        ASSERT_EQ(state->c, 4);
    }
};

struct SystemScheduleTest_Context : public State {
    bool system1Executed;
    bool system2Executed;
    bool system3Executed;
};

struct SystemScheduleTest_WorldSetupSystem : public SetupSystem {
public:
    void Execute(Runtime::SystemCommands& commands) override
    {
        commands.EmplaceState<SystemScheduleTest_Context>(SystemScheduleTest_Context { {}, false, false, false });
    }
};

struct SystemScheduleTest_System1 : public TickSystem {
public:
    void Execute(Runtime::SystemCommands& commands) override
    {
        ASSERT_TRUE(commands.HasState<SystemScheduleTest_Context>());
        const auto* context = commands.GetState<SystemScheduleTest_Context>();
        ASSERT_FALSE(context->system1Executed);
        ASSERT_FALSE(context->system2Executed);
        ASSERT_FALSE(context->system3Executed);

        commands.PatchState<SystemScheduleTest_Context>([](SystemScheduleTest_Context& context) -> void
        {
            context.system1Executed = true;
        });
    }
};

struct SystemScheduleTest_System2 : public TickSystem {
public:
    void Execute(Runtime::SystemCommands& commands) override
    {
        ASSERT_TRUE(commands.HasState<SystemScheduleTest_Context>());
        const auto* context = commands.GetState<SystemScheduleTest_Context>();
        ASSERT_TRUE(context->system1Executed);
        ASSERT_FALSE(context->system2Executed);
        ASSERT_FALSE(context->system3Executed);

        commands.PatchState<SystemScheduleTest_Context>([](SystemScheduleTest_Context& context) -> void
        {
            context.system2Executed = true;
        });
    }
};

struct SystemScheduleTest_System3 : public TickSystem {
public:
    void Execute(Runtime::SystemCommands& commands) override
    {
        ASSERT_TRUE(commands.HasState<SystemScheduleTest_Context>());
        const auto* context = commands.GetState<SystemScheduleTest_Context>();
        ASSERT_TRUE(context->system1Executed);
        ASSERT_TRUE(context->system2Executed);
        ASSERT_FALSE(context->system3Executed);

        commands.PatchState<SystemScheduleTest_Context>([](SystemScheduleTest_Context& context) -> void
        {
            context.system3Executed = true;
        });
    }
};

struct EventTest_GlobalStatus : public State {
    bool stateAdded;
    bool stateUpdated;
    bool stateRemoved;
    Entity testEntity;
    bool componentAdded;
    bool componentUpdated;
    bool componentRemoved;
};

struct EClass() EventTest_EmptyState : public State {
    EStateBody(EventTest_EmptyState)
};

struct EClass() EventTest_EmptyComponent : public Component {
    ECompBody(EventTest_EmptyComponent)
};

struct EventTest_WorldSetupSystem : public SetupSystem {
    void Execute(Runtime::SystemCommands& commands) override
    {
        Entity testEntity = commands.Create();
        commands.EmplaceState<EventTest_GlobalStatus>(EventTest_GlobalStatus {
            {},
            false,
            false,
            false,
            testEntity,
            false,
            false,
            false });

        commands.EmplaceState<EventTest_EmptyState>();
        commands.Emplace<EventTest_EmptyComponent>(testEntity, EventTest_EmptyComponent { {} });
    }
};

struct EventTest_WorldTickSystem : public TickSystem {
    void Execute(Runtime::SystemCommands& commands) override
    {
        const auto* globalState = commands.GetState<EventTest_GlobalStatus>();
        if (globalState->stateUpdated) {
            ASSERT_TRUE(globalState->stateAdded);
        }
        if (globalState->stateRemoved) {
            ASSERT_TRUE(globalState->stateAdded);
            ASSERT_TRUE(globalState->stateUpdated);
        }
        if (globalState->componentUpdated) {
            ASSERT_TRUE(globalState->componentAdded);
        }
        if (globalState->componentRemoved) {
            ASSERT_TRUE(globalState->componentAdded);
            ASSERT_TRUE(globalState->componentUpdated);
        }

        if (globalState->stateAdded && !globalState->stateUpdated) {
            commands.UpdatedState<EventTest_EmptyState>();
        }
        if (globalState->stateUpdated && !globalState->stateRemoved) {
            commands.RemoveState<EventTest_EmptyState>();
        }
        if (globalState->componentAdded && !globalState->componentUpdated) {
            commands.Updated<EventTest_EmptyComponent>(globalState->testEntity);
        }
        if (globalState->componentUpdated && !globalState->componentRemoved) {
            commands.Remove<EventTest_EmptyComponent>(globalState->testEntity);
        }
    }
};

struct EClass() EventTest_OnStateAddedSystem : public EventSystem {
    EStateBody(EventTest_OnStateAddedSystem)

    void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
    {
        auto* globalStatus = commands.GetState<EventTest_GlobalStatus>();
        globalStatus->stateAdded = true;
        commands.UpdatedState<EventTest_GlobalStatus>();
    }
};

struct EventTest_OnStateUpdatedSystem : public EventSystem {
    void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
    {
        auto* globalStatus = commands.GetState<EventTest_GlobalStatus>();
        globalStatus->stateUpdated = true;
        commands.UpdatedState<EventTest_GlobalStatus>();
    }
};

struct EventTest_OnStateRemoveSystem : public EventSystem {
    void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
    {
        auto* globalStatus = commands.GetState<EventTest_GlobalStatus>();
        globalStatus->stateRemoved = true;
        commands.UpdatedState<EventTest_GlobalStatus>();
    }
};

struct EventTest_OnComponentAddedSystem : public EventSystem {
    void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
    {
        EventDecoder<EventTest_EmptyComponent::Added> decoder(eventRef);
        Entity entity = commands.GetState<EventTest_GlobalStatus>()->testEntity;
        ASSERT_EQ(entity, decoder.Get().entity);

        auto* globalStatus = commands.GetState<EventTest_GlobalStatus>();
        globalStatus->componentAdded = true;
        commands.UpdatedState<EventTest_GlobalStatus>();
    }
};

struct EventTest_OnComponentUpdatedSystem : public EventSystem {
    void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
    {
        EventDecoder<EventTest_EmptyComponent::Updated> decoder(eventRef);
        Entity entity = commands.GetState<EventTest_GlobalStatus>()->testEntity;
        ASSERT_EQ(entity, decoder.Get().entity);

        auto* globalStatus = commands.GetState<EventTest_GlobalStatus>();
        globalStatus->componentUpdated = true;
        commands.UpdatedState<EventTest_GlobalStatus>();
    }
};

struct EventTest_OnComponentRemovedSystem : public EventSystem {
    void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
    {
        EventDecoder<EventTest_EmptyComponent::Removed> decoder(eventRef);
        Entity entity = commands.GetState<EventTest_GlobalStatus>()->testEntity;
        ASSERT_EQ(entity, decoder.Get().entity);

        auto* globalStatus = commands.GetState<EventTest_GlobalStatus>();
        globalStatus->componentRemoved = true;
        commands.UpdatedState<EventTest_GlobalStatus>();
    }
};

struct CustomEventTest_CustomEvent {
    int value;
};

struct CustomEventTest_WorldSetupSystem : public SetupSystem {
    void Execute(Runtime::SystemCommands& commands) override
    {
        commands.Broadcast(CustomEventTest_CustomEvent { 1 });
    }
};

struct CustomEventTest_CustomEventSystem : public EventSystem {
    void OnReceiveEvent(Runtime::SystemCommands& commands, const Mirror::Any& eventRef) override
    {
        EventDecoder<CustomEventTest_CustomEvent> decoder(eventRef);
        ASSERT_EQ(decoder.Get().value, 1);
    }
};
