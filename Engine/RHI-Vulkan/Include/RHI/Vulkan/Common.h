//
// Created by johnk on 11/1/2022.
//

#ifndef EXPLOSION_RHI_VULKAN_H
#define EXPLOSION_RHI_VULKAN_H

#include <stdexcept>
#include <format>

#include <vulkan/vulkan.hpp>

#include <RHI/Enum.h>

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

// duplicated code because static variable and namespace
namespace RHI::Vulkan {
    template <typename A, typename B>
    static const std::unordered_map<A, B> VK_ENUM_MAP;

    template <typename A, typename B>
    B VKEnumCast(const A& value)
    {
        auto iter = VK_ENUM_MAP<A, B>.find(value);
        if (iter == VK_ENUM_MAP<A, B>.end()) {
            throw VKException(std::format("failed to find suitable enum cast result for {}", typeid(A).name()));
        }
        return static_cast<B>(iter->second);
    }

#define VK_ENUM_MAP_BEGIN(A, B) template <> static const std::unordered_map<A, B> VK_ENUM_MAP<A, B> = {
#define VK_ENUM_MAP_ITEM(A, B) { A, B },
#define VK_ENUM_MAP_END() };
}

// enum map definitions
namespace RHI::Vulkan {
    VK_ENUM_MAP_BEGIN(vk::PhysicalDeviceType, GpuType)
        VK_ENUM_MAP_ITEM(vk::PhysicalDeviceType::eOther,         GpuType::SOFTWARE)
        VK_ENUM_MAP_ITEM(vk::PhysicalDeviceType::eIntegratedGpu, GpuType::HARDWARE)
        VK_ENUM_MAP_ITEM(vk::PhysicalDeviceType::eDiscreteGpu,   GpuType::HARDWARE)
        VK_ENUM_MAP_ITEM(vk::PhysicalDeviceType::eVirtualGpu,    GpuType::SOFTWARE)
        VK_ENUM_MAP_ITEM(vk::PhysicalDeviceType::eCpu,           GpuType::SOFTWARE)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(QueueType, vk::QueueFlagBits)
        VK_ENUM_MAP_ITEM(QueueType::GRAPHICS, vk::QueueFlagBits::eGraphics)
        VK_ENUM_MAP_ITEM(QueueType::COMPUTE, vk::QueueFlagBits::eCompute)
        VK_ENUM_MAP_ITEM(QueueType::TRANSFER, vk::QueueFlagBits::eCompute)
    VK_ENUM_MAP_END()
}

#endif //EXPLOSION_RHI_VULKAN_H
