//
// Created by johnk on 2023/8/19.
//

#pragma once

#include <Runtime/ECS.h>

struct EClass() Position : public Runtime::Component {
    EClassBody(Position)

    Position() : Runtime::Component() {}

    Position(float inX, float inY) : Runtime::Component(), x(inX), y(inY) {}

    float x;
    float y;
};

struct EClass() Velocity : public Runtime::Component {
    EClassBody(Velocity)

    Velocity(float inX, float inY) : Runtime::Component(), x(inX), y(inY) {}

    float x;
    float y;
};

class EClass() VelocitySystem : public Runtime::System {
public:
    EClassBody(VelocitySystem)
    DefineWaitSystemTypes()

    VelocitySystem() = default;
    ~VelocitySystem() override = default;

    void Setup() {}

    void Tick(const Runtime::Query<Position, Velocity>& query) // NOLINT
    {
        query.ForEach([](Position& position, Velocity& velocity) -> void {
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

    void Setup(const Runtime::Query<Position>& query)
    {
        query.ForEach([this](Position& position) -> void {
            position.x = x;
            position.y = y;
        });
    }

    void Tick() {}

private:
    float x;
    float y;
};
