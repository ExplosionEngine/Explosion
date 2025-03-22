//
// Created by johnk on 2024/12/25.
//

#pragma once

#include <unordered_map>

#include <Runtime/Meta.h>
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

struct EClass(globalComp) GBasicTest_ExpectVerifyResult {
    EClassBody(GBasicTest_ExpectVerifyResult)

    std::unordered_map<Runtime::Entity, Position> entities;
};

class EClass() BasicTest_MotionSystem : public Runtime::System {
    EPolyClassBody(BasicTest_MotionSystem)

    explicit BasicTest_MotionSystem(Runtime::ECRegistry& inRegistry, const Runtime::SystemSetupContext& inContext);
    ~BasicTest_MotionSystem() override;

    void Tick(float inDeltaTimeSeconds) override;
};

struct EClass(globalComp) GConcurrentTest_Context {
    EClassBody(GConcurrentTest_Context)

    uint32_t a;
    uint32_t b;
    uint32_t sum;
    uint32_t tickCount;
};

struct EClass() ConcurrentTest_SystemA : public Runtime::System {
    EPolyClassBody(ConcurrentTest_SystemA)

    explicit ConcurrentTest_SystemA(Runtime::ECRegistry& inRegistry, const Runtime::SystemSetupContext& inContext);
    ~ConcurrentTest_SystemA() override;

    void Tick(float inDeltaTimeSeconds) override;
};

struct EClass() ConcurrentTest_SystemB : public Runtime::System {
    EPolyClassBody(ConcurrentTest_SystemB)

    explicit ConcurrentTest_SystemB(Runtime::ECRegistry& inRegistry, const Runtime::SystemSetupContext& inContext);
    ~ConcurrentTest_SystemB() override;

    void Tick(float inDeltaTimeSeconds) override;
};

struct EClass() ConcurrentTest_VerifySystem : public Runtime::System {
    EPolyClassBody(ConcurrentTest_VerifySystem)

    explicit ConcurrentTest_VerifySystem(Runtime::ECRegistry& inRegistry, const Runtime::SystemSetupContext& inContext);
    ~ConcurrentTest_VerifySystem() override;

    void Tick(float inDeltaTimeSeconds) override;
};
