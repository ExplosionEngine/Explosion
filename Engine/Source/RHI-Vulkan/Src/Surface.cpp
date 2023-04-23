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
            device.GetGpu().GetInstance().GetVkInstance().destroySurfaceKHR(vkSurface);
        }
    }

    void VKSurface::Destroy()
    {
        delete this;
    }

    vk::SurfaceKHR VKSurface::GetVKSurface() const
    {
        return vkSurface;
    }
}
