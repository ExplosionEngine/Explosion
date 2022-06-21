//
// Created by johnk on 2022/6/7.
//

#include <Common/Meta.h>

struct Meta(Class) S0 {
    Meta(Property) int a;
    Meta(Property) float b;
    Meta(Property) double c;
};

struct Meta(Class) S1 {
public:
    Meta(Function) S1(int inA, float inB, double inC)
        : a(inA), b(inB), c(inC) {}
    Meta(Function) ~S1() = default;

    Meta(Property) double c;

protected:
    float b;

private:
    int a;
};

struct Meta(Class) S2 {
public:
    Meta(Function) S2(int inA, float inB) : a(inA), b(inB) {}

    Meta(Function) int GetA() { return a; }
    Meta(Function) float* GetPointerB(int t) { return &b; }

private:
    int a;
    float b;
};

class Meta(Class) C0 {
public:
    Meta(Function) explicit C0(int inA) : a(inA) {}

    Meta(Function) int* GetA(float* b) { return &a; }

private:
    int a;
};