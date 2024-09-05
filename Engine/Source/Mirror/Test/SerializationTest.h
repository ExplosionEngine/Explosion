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
    a EMeta(test=true),
    b,
    max
};

EProperty() extern int ga;
EProperty() extern float gb;
EProperty() extern std::string gc;

struct EClass() SerializationTestStruct0 {
    EClassBody(SerializationTestStruct0)

    EProperty() int a;
    EProperty() float b;
    EProperty() std::string c;
};

struct EClass() SerializationTestStruct1 {
    EClassBody(SerializationTestStruct1)

    EProperty() std::vector<int> a;
    EProperty() std::unordered_set<std::string> b;
    EProperty() std::unordered_map<int, std::string> c;
    EProperty() std::vector<std::vector<bool>> d;
};
