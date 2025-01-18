//
// Created by Zach Lee on 2022/4/4.
//

#if PLATFORM_MACOS
#include <Cocoa/Cocoa.h>

#include <RHI/Surface.h>
#include <Common/Debug.h>

#define VK_USE_PLATFORM_METAL_EXT
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

        VkMetalSurfaceCreateInfoEXT surfaceInfo = {};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
        surfaceInfo.pLayer = static_cast<CAMetalLayer*>(layer);

        VkSurfaceKHR surface = VK_NULL_HANDLE;
        vkCreateMetalSurfaceEXT(instance, &surfaceInfo, nullptr, &surface);
        return surface;
    }
}
#endif
