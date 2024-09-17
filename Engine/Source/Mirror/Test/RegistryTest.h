//
// Created by johnk on 2024/9/4.
//

#pragma once

#include <Mirror/Meta.h>

EProperty(testKey=v0) extern int v0;

EFunc() int F0(const int a, const int b);
EFunc() int& F1();
EFunc() void F2(int& outValue);

struct EClass(testKey=C0) C0 {
    EClassBody(C0)

    EFunc(testKey=F0) static int& F0();
    EProperty(testKey=v0) static int v0;
};

class EClass() C1 {
public:
    EClassBody(C1)

    explicit C1(const int inV0);

    EFunc() int GetV0() const;
    EFunc() void SetV0(const int inV0);

private:
    EProperty() int v0;
};

struct EClass() C2 {
    EClassBody(C2)

    C2(const int inA, const int inB);

    EProperty() int a;
    EProperty() int b;
};

enum class EEnum() E0 {
    a,
    b,
    c,
    max
};

struct EClass() C3 : C2 {
    EClassBody(C3)

    C3(const int inA, const int inB, const int inC);

    EProperty() int c;
};
