//
// Created by johnk on 2023/4/17.
//

#pragma once

#include <vulkan/vulkan.h>

#include <RHI/Surface.h>

namespace RHI::Vulkan {
    class VKDevice;
    VkSurfaceKHR CreateNativeSurface(const VkInstance& instance, const SurfaceCreateInfo& createInfo);

    class VKSurface : public Surface {
    public:
        NON_COPYABLE(VKSurface)
        VKSurface(VKDevice& inDevice, const SurfaceCreateInfo& inCreateInfo);
        ~VKSurface() override;

        void Destroy() override;
        VkSurfaceKHR GetVKSurface() const;

    private:
        VKDevice& device;
        VkSurfaceKHR vkSurface;
    };
}
