//
// Created by johnk on 2024/9/4.
//

#pragma once

#include <cstdint>
#include <string>

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

struct AnyCopyAssignTest {
    AnyCopyAssignTest();
    AnyCopyAssignTest(AnyCopyAssignTest&& inOther) noexcept;
    AnyCopyAssignTest& operator=(const AnyCopyAssignTest& inOther);

    bool called;
};

struct AnyMoveAssignTest {
    AnyMoveAssignTest();
    AnyMoveAssignTest(AnyMoveAssignTest&& inOther) noexcept;
    AnyMoveAssignTest& operator=(AnyMoveAssignTest&& inOther) noexcept;

    bool called;
};

struct AnyBasicTest {
    int a;
    float b;

    bool operator==(const AnyBasicTest& inRhs) const;
};

struct EClass() AnyBaseClassTest {
    EPolyClassBody(AnyBaseClassTest)

    virtual ~AnyBaseClassTest() = default;
};

struct EClass() AnyDerivedClassTest : AnyBaseClassTest {
    EPolyClassBody(AnyDerivedClassTest)

    ~AnyDerivedClassTest() override = default;
};

struct EClass() AnyBaseClassTest2 {
    EPolyClassBody(AnyBaseClassTest2)

    AnyBaseClassTest2(int inA, float inB)
        : a(inA)
        , b(inB)
    {
    }

    virtual ~AnyBaseClassTest2() = default;

    int a;
    float b;
};

struct EClass() AnyDerivedClassTest2 final : AnyBaseClassTest2 {
    EPolyClassBody(AnyDerivedClassTest2)

    AnyDerivedClassTest2(int inA, float inB, std::string inC)
        : AnyBaseClassTest2(inA, inB)
        , c(std::move(inC))
    {
    }

    ~AnyDerivedClassTest2() override = default;

    std::string c;
};
