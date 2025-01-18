//
// Created by johnk on 2024/12/25.
//

#pragma once

#include <unordered_map>

#include <Mirror/Meta.h>
#include <Runtime/ECS.h>
#include <Common/Math/Common.h>

struct EClass() Position {
    EClassBody(Position)

    Position();
    Position(float inX, float inY);

    bool operator==(const Position& inRhs) const;

    float x;
    float y;
};

struct EClass() Velocity {
    EClassBody(Velocity)

    Velocity();
    Velocity(float inX, float inY);

    bool operator==(const Velocity& inRhs) const;

    float x;
    float y;
};

struct EClass() BasicTest_ExpectVerifyResult {
    EClassBody(BasicTest_ExpectVerifyResult)

    std::unordered_map<Runtime::Entity, Position> entities;
};

class EClass() BasicTest_MotionSystem : public Runtime::System {
    EClassBody(BasicTest_MotionSystem)

    explicit BasicTest_MotionSystem(Runtime::ECRegistry& inRegistry);
    ~BasicTest_MotionSystem() override;

    void Tick(float inDeltaTimeMs) override;
};

struct EClass() ConcurrentTest_Context {
    EClassBody(ConcurrentTest_Context)

    uint32_t a;
    uint32_t b;
    uint32_t sum;
    uint32_t tickCount;
};

struct EClass() ConcurrentTest_SystemA : public Runtime::System {
    EClassBody(ConcurrentTest_SystemA)

    explicit ConcurrentTest_SystemA(Runtime::ECRegistry& inRegistry);
    ~ConcurrentTest_SystemA() override;

    void Tick(float inDeltaTimeMs) override;
};

struct EClass() ConcurrentTest_SystemB : public Runtime::System {
    EClassBody(ConcurrentTest_SystemB)

    explicit ConcurrentTest_SystemB(Runtime::ECRegistry& inRegistry);
    ~ConcurrentTest_SystemB() override;

    void Tick(float inDeltaTimeMs) override;
};

struct EClass() ConcurrentTest_VerifySystem : public Runtime::System {
    EClassBody(ConcurrentTest_VerifySystem)

    explicit ConcurrentTest_VerifySystem(Runtime::ECRegistry& inRegistry);
    ~ConcurrentTest_VerifySystem() override;

    void Tick(float inDeltaTimeMs) override;
};
