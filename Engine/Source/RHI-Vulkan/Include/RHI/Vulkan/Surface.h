//
// Created by johnk on 2023/4/17.
//

#pragma once

#include <vulkan/vulkan.h>

#include <RHI/Surface.h>

namespace RHI::Vulkan {
    class VulkanDevice;

    VkSurfaceKHR CreateNativeSurface(const VkInstance& instance, const SurfaceCreateInfo& createInfo);

    class VulkanSurface : public Surface {
    public:
        NonCopyable(VulkanSurface)
        VulkanSurface(VulkanDevice& inDevice, const SurfaceCreateInfo& inCreateInfo);
        ~VulkanSurface() override;

        VkSurfaceKHR GetNative() const;

    private:
        VulkanDevice& device;
        VkSurfaceKHR nativeSurface;
    };
}
