//
// Created by johnk on 2024/9/5.
//

#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <Mirror/Meta.h>

enum class EEnum(test=true) SerializationTestEnum {
    a EMeta(test=a),
    b EMeta(test=b),
    max EMeta(test=c)
};

EProperty() extern int ga;
EProperty() extern float gb;
EProperty() extern std::string gc;

EFunc() int gf(int a, int b);

struct EClass() SerializationTestStruct0 {
    EClassBody(SerializationTestStruct0)

    EProperty() int a;
    EProperty() float b;
    EProperty() std::string c;

    bool operator==(const SerializationTestStruct0& rhs) const
    {
        return a == rhs.a
            && b == rhs.b
            && c == rhs.c;
    }
};

struct EClass() SerializationTestStruct1 {
    EClassBody(SerializationTestStruct1)

    EProperty() std::vector<int> a;
    EProperty() std::unordered_set<std::string> b;
    EProperty() std::unordered_map<int, std::string> c;
    EProperty() std::vector<std::vector<bool>> d;
    EProperty() std::vector<SerializationTestStruct0> e;

    bool operator==(const SerializationTestStruct1& rhs) const
    {
        return a == rhs.a
            && b == rhs.b
            && c == rhs.c
            && d == rhs.d
            && e == rhs.e;
    }
};

struct EClass() SerializationTestStruct2 : SerializationTestStruct0 {
    EClassBody(SerializationTestStruct2);

    EProperty() double d;

    bool operator==(const SerializationTestStruct2& rhs) const
    {
        return SerializationTestStruct0::operator==(rhs)
            && d == rhs.d;
    }
};

struct EClass() SerializationTestStruct3 {
    EClassBody(SerializationTestStruct3)

    EProperty() static int ga;
    EFunc() static int gf(int a, int b);

    EProperty() int a;
    EFunc() int f() const;
};
