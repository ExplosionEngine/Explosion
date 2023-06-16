//
// Created by johnk on 2023/4/17.
//

#include <RHI/Vulkan/Surface.h>
#include <RHI/Vulkan/Instance.h>
#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/Device.h>

namespace RHI::Vulkan {
    VKSurface::VKSurface(VKDevice& inDevice, const RHI::SurfaceCreateInfo& inCreateInfo)
        : Surface(inCreateInfo)
        , device(inDevice)
    {
        vkSurface = CreateNativeSurface(device.GetGpu().GetInstance().GetVkInstance(), inCreateInfo);
    }

    VKSurface::~VKSurface()
    {
        if (vkSurface) {
            vkDestroySurfaceKHR(device.GetGpu().GetInstance().GetVkInstance(), vkSurface, nullptr);
        }
    }

    void VKSurface::Destroy()
    {
        delete this;
    }

    VkSurfaceKHR VKSurface::GetVKSurface() const
    {
        return vkSurface;
    }
}
