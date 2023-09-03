//
// Created by johnk on 2023/8/19.
//

#pragma once

#include <Runtime/ECS.h>

struct EClass() PositionComponent : public Runtime::Component {
    EClassBody(PositionComponent)

    PositionComponent() = default;

    PositionComponent(float inX, float inY) : x(inX), y(inY) {}

    float x;
    float y;
};

struct EClass() VelocityComponent : public Runtime::Component {
    EClassBody(VelocityComponent)

    VelocityComponent(float inX, float inY) : x(inX), y(inY) {}

    float x;
    float y;
};

struct EClass() LifecycleTestComponent : public Runtime::Component {
    EClassBody(LifecycleTestComponent)

    LifecycleTestComponent(std::function<void()> inOnConstructCall, std::function<void()> inOnDestroyCall)
        : onConstructCall(std::move(inOnConstructCall))
        , onDestroyCall(std::move(inOnDestroyCall))
    {
    }

    void OnConstruct() override
    {
        onConstructCall();
    }

    void OnDestroy() override
    {
        onDestroyCall();
    }

    std::function<void()> onConstructCall;
    std::function<void()> onDestroyCall;
};

class EClass() VelocitySystem : public Runtime::System {
public:
    EClassBody(VelocitySystem)
    DefineWaitSystemTypes()

    VelocitySystem() = default;
    ~VelocitySystem() override = default;

    void Setup() {}

    void Tick(const Runtime::Query<PositionComponent, VelocityComponent>& query) // NOLINT
    {
        query.ForEach([](PositionComponent& position, VelocityComponent& velocity) -> void {
            position.x += velocity.x;
            position.y += velocity.y;
        });
    }
};

class EClass() PositionSetupSystem : public Runtime::System {
public:
    EClassBody(PositionSetupSystem)
    DefineWaitSystemTypes()

    PositionSetupSystem(float inX, float inY) : x(inX), y(inY) {}
    ~PositionSetupSystem() override = default;

    void Setup(const Runtime::Query<PositionComponent>& query)
    {
        query.ForEach([this](PositionComponent& position) -> void {
            position.x = x;
            position.y = y;
        });
    }

    void Tick() {}

private:
    float x;
    float y;
};
