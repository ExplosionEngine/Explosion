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

struct EClass() CompB {
    EClassBody(CompB)

    explicit CompB(float inValue)
        : value(inValue)
    {
    }

    float value;
};

struct EClass() GCompA {
    EClassBody(GCompA)

    explicit GCompA(int inValue)
        : value(inValue)
    {
    }

    int value;
};

struct EClass() GCompB {
    EClassBody(GCompB)

    explicit GCompB(float inValue)
        : value(inValue)
    {
    }

    float value;
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
