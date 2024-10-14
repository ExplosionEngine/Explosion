//
// Created by johnk on 2024/8/20.
//

#pragma once

#include <Mirror/Meta.h>
#include <Runtime/World.h>

using namespace Runtime;

class EClass() GlobalCounter {
    EClassBody(GlobalCounter)

    GlobalCounter();

    EProperty() uint32_t tickTime;
    EProperty() uint32_t value;
};

class EClass() ParralCountSystemA final : public System {
    EPolyClassBody(ParralCountSystemA)

    ParralCountSystemA();
    ~ParralCountSystemA() override;

    EFunc() void Tick(Commands& commands, float inTimeMs) const override;
};

class EClass() ParralCountSystemB final : public System {
    EPolyClassBody(ParralCountSystemB)

    ParralCountSystemB();
    ~ParralCountSystemB() override;

    EFunc() void Tick(Commands& commands, float inTimeMs) const override;
};

class EClass() GlobalCounterVerifySystem final : public System {
    EPolyClassBody(GlobalCounterVerifySystem)

    GlobalCounterVerifySystem();
    ~GlobalCounterVerifySystem() override;

    EFunc() void Setup(Commands& commands) const override;
    EFunc() void Tick(Commands& commands, float inTimeMs) const override;
};
