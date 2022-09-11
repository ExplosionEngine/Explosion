//
// Created by Zach Lee on 2022/9/11.
//

#pragma once

namespace Common {

    struct Euler {
        union {
            float v[3];
            struct {
                float x;
                float y;
                float z;
            };
        };

        inline Euler() : Euler(0, 0, 0)
        {
        }

        inline Euler(float x_, float y_, float z_) : x(x_), y(y_), z(z_)
        {
        }
    };
} // namespace Common