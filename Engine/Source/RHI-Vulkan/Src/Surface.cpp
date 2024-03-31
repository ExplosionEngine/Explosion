//
// Created by johnk on 2023/4/17.
//

#include <RHI/Vulkan/Surface.h>
#include <RHI/Vulkan/Instance.h>
#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/Device.h>

namespace RHI::Vulkan {
    VulkanSurface::VulkanSurface(VulkanDevice& inDevice, const RHI::SurfaceCreateInfo& inCreateInfo)
        : Surface(inCreateInfo)
        , device(inDevice)
    {
        nativeSurface = CreateNativeSurface(device.GetGpu().GetInstance().GetNative(), inCreateInfo);
    }

    VulkanSurface::~VulkanSurface()
    {
        if (nativeSurface) {
            vkDestroySurfaceKHR(device.GetGpu().GetInstance().GetNative(), nativeSurface, nullptr);
        }
    }

    void VulkanSurface::Destroy()
    {
        delete this;
    }

    VkSurfaceKHR VulkanSurface::GetNative() const
    {
        return nativeSurface;
    }
}
