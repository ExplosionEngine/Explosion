//
// Created by johnk on 2024/9/4.
//

#pragma once

#include <Mirror/Meta.h>

EProperty(testKey=v0) extern int v0;

EFunc() int F0(const int a, const int b);
EFunc() int& F1();
EFunc() void F2(int& outValue);
EFunc() int F3(int&& inValue);
EFunc() int F4(int inValue);
EFunc() float F4(int inValue, float inRet);

struct EClass(testKey=C0) C0 {
    EClassBody(C0)

    EFunc(testKey=F0) static int& F0();
    EFunc(testKey=F1) static int F1(int inValue);
    EFunc(testKey=F1) static int F1(int inValue0, int inValue1);
    EFunc(testKey=F2) int F2(int inValue);
    EFunc(testKey=F2) int F2(int inValue0, int inValue1);

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

// Fixture with a default constructor + a category meta + a base, for testing
// Class::GetDefaultObject / Class::FindWithCategory / Class::Cast etc.
struct EClass(category=animal) C4 {
    EClassBody(C4)

    EProperty() int v;
};

// Another fixture in the same "animal" category to make FindWithCategory non-trivial.
struct EClass(category=animal) C5 {
    EClassBody(C5)

    EProperty() int w;
};

// Fixture in a different category to verify the filter does discriminate.
struct EClass(category=plant) C6 {
    EClassBody(C6)
};

// Fixture with a transient member, for testing MemberVariable::IsTransient.
struct EClass() C7 {
    EClassBody(C7)

    EProperty() int normal;
    EProperty(transient=true) int trans;
};

// Fixture with float / int / bool / int64 metas, for ReflNode::GetMetaInt32/Int64/Float etc.
struct EClass(boolMeta=true, intMeta=42, int64Meta=1234567890123, floatMeta=3.5) C8 {
    EClassBody(C8)
};
