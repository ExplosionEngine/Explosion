//
// Created by johnk on 2024/7/28.
//

#pragma once

#include <gtest/gtest.h>

namespace Test {
    template <typename T>
    concept Vec = requires (T container)
    {
        container.size();
        container[0];
    };

    template <Vec A, Vec B>
    void AssertVecEq(A&& a, B&& b)
    {
        ASSERT_EQ(a.size(), b.size());
        for (auto i = 0; i < a.size(); i++) {
            ASSERT_EQ(a[i], b[i]);
        }
    }
}
