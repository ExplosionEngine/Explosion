//
// Created by Administrator on 2021/4/4 0004.
//

#ifndef EXPLOSION_UTILS_H
#define EXPLOSION_UTILS_H

#include <vector>
#include <string>
#include <functional>

#include <vulkan/vulkan.h>

namespace Explosion {
    template <typename T>
    using NameComparer = std::function<bool(const char*, const T&)>;

    template <typename T>
    bool CheckPropertySupport(
        const std::vector<const char*>& needs,
        const std::vector<T>& properties,
        const NameComparer<T>& comparer
    ) {
        for (auto& need : needs) {
            bool found = false;
            for (auto& property : properties) {
                if (comparer(need, property)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }
        return true;
    }
}

#endif //EXPLOSION_UTILS_H
