//
// Created by Administrator on 2021/4/4 0004.
//

#ifndef EXPLOSION_UTILS_H
#define EXPLOSION_UTILS_H

#include <vector>
#include <algorithm>
#include <string>

#include <vulkan/vulkan.h>

namespace Explosion {
    bool CheckExtensionSupported(
        const std::vector<const char*>& extensions,
        const std::vector<VkExtensionProperties>& properties
    ) {
        for (auto& extension : extensions) {
            bool found = false;
            for (auto& property : properties) {
                if (std::string(extension) == property.extensionName) {
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
