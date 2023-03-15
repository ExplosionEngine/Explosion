//
// Created by johnk on 2022/12/12.
//

#pragma once

#include <Mirror/Meta.h>

EVar()
int gv0;

EVar()
float gv1;

EFunc()
int gf0(int a, int b) {
    return a + b;
}

struct EClass() C0 {
public:
    EVar()
    static int sv0;

    EVar()
    int v0;

    EVar()
    float v1;

    EFunc()
    static void sf0() {}

    EFunc()
    int f0() { return 0; }
};
