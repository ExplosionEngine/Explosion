//
// Created by johnk on 2023/12/4.
//

#pragma once

#include <vector>
#include <unordered_set>
#include <algorithm>

namespace Common {
    class VectorUtils {
    public:
        template <typename T>
        static inline typename std::vector<T>::iterator SwapWithLastAndDelete(std::vector<T>& vector, const typename std::vector<T>::iterator& iterator);

        template <typename T>
        static inline size_t SwapWithLastAndDelete(std::vector<T>& vector, size_t index);

        template <typename T>
        static inline std::vector<T> GetIntersection(const std::vector<T>& lhs, const std::vector<T>& rhs);
    };

    class SetUtils {
    public:
        template <typename T>
        static inline std::unordered_set<T> GetIntersection(const std::unordered_set<T>& lhs, const std::unordered_set<T>& rhs);
    };
}

namespace Common {
    template <typename T>
    typename std::vector<T>::iterator VectorUtils::SwapWithLastAndDelete(std::vector<T>& vector, const typename std::vector<T>::iterator& iterator)
    {
        std::iter_swap(iterator, vector.end() - 1);
        vector.pop_back();
        return iterator;
    }

    template <typename T>
    size_t VectorUtils::SwapWithLastAndDelete(std::vector<T>& vector, size_t index)
    {
        std::swap(vector[index], vector[vector.size() - 1]);
        vector.pop_back();
        return index;
    }

    template <typename T>
    std::vector<T> VectorUtils::GetIntersection(const std::vector<T>& lhs, const std::vector<T>& rhs)
    {
        std::vector<T> result;
        std::set_intersection(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::back_inserter(result));
        return result;
    }

    template <typename T>
    std::unordered_set<T> SetUtils::GetIntersection(const std::unordered_set<T>& lhs, const std::unordered_set<T>& rhs)
    {
        std::unordered_set<T> result;
        result.reserve(lhs.size());
        for (const auto& elementLhs : lhs) {
            for (const auto& elementRhs : rhs) {
                if (elementLhs == elementRhs) {
                    result.emplace(elementLhs);
                }
            }
        }
        return result;
    }
}
