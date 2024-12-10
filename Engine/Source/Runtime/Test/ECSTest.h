//
// Created by johnk on 2024/12/9.
//

#pragma once

#include <Mirror/Meta.h>
#include <Runtime/ECS.h>
using namespace Runtime;

struct EClass() CompA {
    EClassBody(CompA)

    explicit CompA(int inValue)
        : value(inValue)
    {
    }

    int value;
};
