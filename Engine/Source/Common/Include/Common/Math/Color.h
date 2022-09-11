//
// Created by Zach Lee on 2022/9/10.
//

#pragma once

namespace Common {

    struct Color {
        union {
            float v[4];
            struct {
                float r;
                float g;
                float b;
                float a;
            };
        };

        Color();
        Color(float r_, float g_, float b_, float a_);
    };

    Color::Color() : Color(0, 0, 0, 0)
    {
    }

    Color::Color(float r_, float g_, float b_, float a_) : r(r_), g(g_), b(b_), a(a_)
    {
    }

} // namespace Common