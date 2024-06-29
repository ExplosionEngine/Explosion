//
// Created by johnk on 2024/6/30.
//

#pragma once

#include <Mirror/Meta.h>
#include <Runtime/GameObject.h>

using namespace Runtime;

struct EClass() Velocity : Component {
    EClassBody(Velocity)

    ECtor() Velocity() : x(0), y(0) {}
    ECtor() Velocity(float inX, float inY) : x(inX), y(inY) {}
    EProperty() float x;
    EProperty() float y;
};

struct EClass() Position : Component {
    EClassBody(Velocity)

    ECtor() Position() : x(0), y(0) {}
    ECtor() Position(float inX, float inY) : x(inX), y(inY) {}
    EProperty() float x;
    EProperty() float y;
};
