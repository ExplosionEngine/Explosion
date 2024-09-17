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

    template <StdVec A, StdVec B> void AssertVecEq(A&& inLhs, B&& inRhs);
    template <ExpVec A, StdVec B> void AssertVecEq(A&& inLhs, B&& inRhs);
    template <StdVec A, ExpVec B> void AssertVecEq(A&& inLhs, B&& inRhs);

    template <typename T> using CustomComparer = std::function<bool(const T&, const T&)>;
    template <typename T> bool CompareVec(const std::vector<T>& inLhs, const std::vector<T>& inRhs);
    template <typename T> bool CompareUnorderedSet(const std::unordered_set<T>& inLhs, const std::unordered_set<T>& inRhs);
    template <typename K, typename V> bool CompareUnorderedMap(const std::unordered_map<K, V>& inLhs, const std::unordered_map<K, V>& inRhs);
}

namespace Test {
    template <StdVec A, StdVec B>
    void AssertVecEq(A&& inLhs, B&& inRhs)
    {
        ASSERT_EQ(inLhs.size(), inRhs.size());
        for (auto i = 0; i < inLhs.size(); i++) {
            ASSERT_EQ(inLhs[i], inRhs[i]);
        }
    }

    template <ExpVec A, StdVec B>
    void AssertVecEq(A&& inLhs, B&& inRhs)
    {
        ASSERT_EQ(inLhs.Size(), inRhs.size());
        for (auto i = 0; i < inLhs.Size(); i++) {
            ASSERT_EQ(inLhs[i], inRhs[i]);
        }
    }

    template <StdVec A, ExpVec B>
    void AssertVecEq(A&& inLhs, B&& inRhs)
    {
        ASSERT_EQ(inLhs.size(), inRhs.Size());
        for (auto i = 0; i < inLhs.size(); i++) {
            ASSERT_EQ(inLhs[i], inRhs[i]);
        }
    }

    template <typename T>
    bool CompareVec(const std::vector<T>& inLhs, const std::vector<T>& inRhs)
    {
        if (inLhs.size() != inRhs.size()) {
            return false;
        }
        for (auto i = 0; i < inLhs.size(); i++) {
            if (inLhs[i] != inRhs[i]) {
                return false;
            }
        }
        return true;
    }

    template <typename T>
    bool CompareUnorderedSet(const std::unordered_set<T>& inLhs, const std::unordered_set<T>& inRhs)
    {
        if (inLhs.size() != inRhs.size()) {
            return false;
        }
        for (const auto& element : inLhs) {
            if (!inRhs.contains(element)) {
                return false;
            }
        }
        return true;
    }

    template <typename K, typename V>
    bool CompareUnorderedMap(const std::unordered_map<K, V>& inLhs, const std::unordered_map<K, V>& inRhs)
    {
        if (inLhs.size() != inRhs.size()) {
            return false;
        }
        for (const auto& [key, value] : inLhs) {
            if (!inRhs.contains(key)) {
                return false;
            }
            if (inRhs.at(key) != value) {
                return false;
            }
        }
        return true;
    }
}
