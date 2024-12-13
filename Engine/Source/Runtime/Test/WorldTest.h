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
