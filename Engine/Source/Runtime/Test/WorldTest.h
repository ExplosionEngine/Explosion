//
// Created by johnk on 2024/8/20.
//

#pragma once

#include <Mirror/Meta.h>
#include <Runtime/World.h>

struct EClass() Position : Runtime::Component {
    EClassBody(Position);

    float x;
    float y;
};

struct EClass() Velocity : Runtime::Component {
    EClassBody(Velocity);

    float x;
    float y;
};

struct EClass() IterTimeCount : Runtime::State {
    EClassBody(IterTimeCount);

    size_t value;
};

struct EClass() StartVerify : Runtime::Event {
    EClassBody(StartVerify);
};

struct EClass() BasicSetupSystem : Runtime::System {
    EClassBody(SetupSystem);

    EFunc() void Execute(Runtime::Commands& commands, const Runtime::WorldStart&);
};

struct EClass() BasicTickSystem : Runtime::System {
    EClassBody(BasicTickSystem);

    EFunc() StartVerify Execute(Runtime::Commands& commands, const Runtime::WorldTick&);
};

struct EClass() PositionVerifySystem : Runtime::System {
    EClassBody(PositionVerifySystem)

    EFunc() void Execute(Runtime::Commands& commands, const StartVerify&);
};
