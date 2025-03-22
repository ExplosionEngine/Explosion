//
// Created by johnk on 2024/12/9.
//

#pragma once

#include <Runtime/Meta.h>
#include <Runtime/ECS.h>
using namespace Runtime;

struct EClass() CompA {
    EClassBody(CompA)

    CompA()
        : value(0)
    {
    }

    explicit CompA(int inValue)
        : value(inValue)
    {
    }

    EProperty() int value;
};

struct EClass() CompB {
    EClassBody(CompB)

    CompB()
        : value(0.0f)
    {
    }

    explicit CompB(float inValue)
        : value(inValue)
    {
    }

    EProperty() float value;
};

struct EClass(globalComp) GCompA {
    EClassBody(GCompA)

    GCompA()
        : value(0)
    {
    }

    explicit GCompA(int inValue)
        : value(inValue)
    {
    }

    EProperty() int value;
};

struct EClass(globalComp) GCompB {
    EClassBody(GCompB)

    GCompB()
        : value(0.0f)
    {
    }

    explicit GCompB(float inValue)
        : value(inValue)
    {
    }

    EProperty() float value;
};

struct EventCounts {
    uint32_t onConstructed;
    uint32_t onUpdated;
    uint32_t onRemove;

    EventCounts()
        : onConstructed(0)
        , onUpdated(0)
        , onRemove(0)
    {
    }

    EventCounts(uint32_t inOnConstructed, uint32_t inOnUpdated, uint32_t inOnRemove)
        : onConstructed(inOnConstructed)
        , onUpdated(inOnUpdated)
        , onRemove(inOnRemove)
    {
    }

    bool operator==(const EventCounts& inRhs) const
    {
        return onConstructed == inRhs.onConstructed
            && onUpdated == inRhs.onUpdated
            && onRemove == inRhs.onRemove;
    }
};
