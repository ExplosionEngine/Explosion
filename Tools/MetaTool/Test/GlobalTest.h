//
// Created by johnk on 2022/6/7.
//

#include <Common/Meta.h>

Meta(Property) int v0;
Meta(Property) float v1;
Meta(Property) double v2;
Meta(Property) int* v3;
Meta(Property) int** v4;

Meta(Function, EditorUI(Alias("TestButton"))) void f0() {}
Meta(Function) int f1() { return 0; }
Meta(Function) float f2(int a, int b) { return 0.0f; }
Meta(Function) double* f3(int* a, int* b) { return nullptr; }
