//
// Created by johnk on 11/1/2022.
//

#ifndef EXPLOSION_RHI_VULKAN_H
#define EXPLOSION_RHI_VULKAN_H

#include <stdexcept>

namespace RHI::Vulkan {
    class VKException : public std::exception {
    public:
        explicit VKException(std::string m) : msg(std::move(m)) {}
        ~VKException() override = default;

        [[nodiscard]] const char* what() const override
        {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
}

namespace RHI::Vulkan {
#if PLATFORM_WINDOWS
#define VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME "VK_KHR_win32_surface"
#endif
#define VK_KHRONOS_VALIDATION_LAYER_NAME "VK_LAYER_KHRONOS_validation"
}

#endif //EXPLOSION_RHI_VULKAN_H
