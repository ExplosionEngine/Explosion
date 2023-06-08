//
// Created by Zach Lee on 2022/4/4.
//

#if PLATFORM_MACOS
#include <RHI/SwapChain.h>
#include <Cocoa/Cocoa.h>
#include <Common/Debug.h>

#define VK_USE_PLATFORM_MACOS_MVK
#include <vulkan/vulkan.h>

namespace RHI::Vulkan {

    VkSurfaceKHR CreateNativeSurface(const VkInstance& instance, const SurfaceCreateInfo& createInfo)
    {
        auto nsWin = static_cast<NSWindow*>(createInfo.window);

        NSBundle* bundle = [NSBundle bundleWithPath: @"/System/Library/Frameworks/QuartzCore.framework"];
        CALayer* layer = [[bundle classNamed: @"CAMetalLayer"] layer];
        NSView* view = nsWin.contentView;
        [view setLayer: layer];
        [view setWantsLayer: YES];

        VkMacOSSurfaceCreateInfoMVK surfaceInfo = {};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
        surfaceInfo.pView = view;
        surfaceInfo.pNext = nullptr;
        surfaceInfo.flags = 0;

        VkSurfaceKHR surface = VK_NULL_HANDLE;
        vkCreateMacOSSurfaceMVK(instance, &surfaceInfo, nullptr, &surface);
        return surface;
    }
}
#endif
