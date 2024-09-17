//
// Created by johnk on 2024/9/4.
//

#pragma once

#include <cstdint>

#include <Mirror/Meta.h>

struct AnyDtorTest {
    explicit AnyDtorTest(bool& inLive);
    AnyDtorTest(bool& inLive, bool inRecord);
    AnyDtorTest(AnyDtorTest&& inOther) noexcept;
    ~AnyDtorTest();

    bool record;
    bool& live;
};

struct AnyCopyCtorTest {
    explicit AnyCopyCtorTest(bool& inCopyCtorCalled);
    AnyCopyCtorTest(AnyCopyCtorTest&& inOther) noexcept;
    AnyCopyCtorTest(const AnyCopyCtorTest& inOther);

    bool& copyCtorCalled;
};

struct AnyMoveCtorTest {
    explicit AnyMoveCtorTest(uint8_t& inMoveTime);
    AnyMoveCtorTest(AnyMoveCtorTest&& inOther) noexcept;

    uint8_t& moveTime;
};

struct AnyBasicTest {
    int a;
    float b;

    bool operator==(const AnyBasicTest& inRhs) const;
};

struct EClass() AnyBaseClassTest {
    EClassBody(AnyBaseClassTest)
};

struct EClass() AnyDerivedClassTest : AnyBaseClassTest {
    EClassBody(AnyDerivedClassTest)
};
