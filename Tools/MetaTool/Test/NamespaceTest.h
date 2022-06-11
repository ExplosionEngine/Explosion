//
// Created by johnk on 2022/6/8.
//

#include <Common/Meta.h>

Meta(Function) int F0() { return 0; };

namespace N0 {
    Meta(Property) int v0;

    Meta(Function) float F1(int a) { return 0.0f; }

    class Meta(Class) C0 {
        int* a;

    public:
        Meta(Function) int** GetA() { return &a; }
    };
}
