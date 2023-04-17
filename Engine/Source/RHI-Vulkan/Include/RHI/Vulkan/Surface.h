//
// Created by johnk on 2023/4/17.
//

#pragma once

#include <vulkan/vulkan.hpp>

#include <RHI/Surface.h>

namespace RHI::Vulkan {
    class VKDevice;
    vk::SurfaceKHR CreateNativeSurface(const vk::Instance& instance, const SurfaceCreateInfo& createInfo);

    class VKSurface : public Surface {
    public:
        NON_COPYABLE(VKSurface)
        VKSurface(VKDevice& inDevice, const SurfaceCreateInfo& inCreateInfo);
        ~VKSurface() override;

        void Destroy() override;
        vk::SurfaceKHR GetVKSurface() const;

    private:
        VKDevice& device;
        vk::SurfaceKHR vkSurface;
    };
}
