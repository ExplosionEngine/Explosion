//
// Created by johnk on 2023/5/15.
//

#pragma once

#include <Common/Math/Vector.h>

namespace Common {
    template <typename T, uint8_t R, uint8_t C>
    struct Matrix {
        // TODO

        T data[R * C];
    };
}
