//
// Created by johnk on 2024/7/28.
//

#pragma once

#include <gtest/gtest.h>

#include <Common/Container.h>

namespace Test {
    template <typename T>
    concept StdVec = requires (T container)
    {
        container.size();
        container[0];
    };

    template <typename T>
    concept ExpVec = requires (T container)
    {
        container.Size();
        container[0];
    };

    template <StdVec A, StdVec B>
    void AssertVecEq(A&& a, B&& b)
    {
        ASSERT_EQ(a.size(), b.size());
        for (auto i = 0; i < a.size(); i++) {
            ASSERT_EQ(a[i], b[i]);
        }
    }

    template <ExpVec A, StdVec B>
    void AssertVecEq(A&& a, B&& b)
    {
        ASSERT_EQ(a.Size(), b.size());
        for (auto i = 0; i < a.Size(); i++) {
            ASSERT_EQ(a[i], b[i]);
        }
    }
}
