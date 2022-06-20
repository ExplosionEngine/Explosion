//
// Created by johnk on 2022/6/8.
//

#include <Common/Meta.h>

namespace N0 {
    class Meta(Class) C0 {
        int* a;

    public:
        Meta(Function) int** GetA() { return &a; }
    };
}
