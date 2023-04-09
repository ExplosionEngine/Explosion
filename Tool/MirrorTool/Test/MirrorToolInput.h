//
// Created by johnk on 2022/12/12.
//

#pragma once

#include <Mirror/Meta.h>

EProperty()
int gv0;

EProperty()
float gv1;

EFunc()
int gf0(int a, int b) {
    return a + b;
}

struct EClass() C0 {
public:
    EProperty()
    static int sv0;

    EProperty(editorHide)
    int v0;

    EProperty()
    float v1;

    EFunc()
    static void sf0() {}

    EFunc()
    int f0() { return 0; }
};
