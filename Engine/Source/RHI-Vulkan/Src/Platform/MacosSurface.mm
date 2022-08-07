//
// Created by Zach Lee on 2022/4/4.
//

#if PLATFORM_MACOS
#include <RHI/SwapChain.h>
#include <Cocoa/Cocoa.h>
#include <Common/Debug.h>

#define VK_USE_PLATFORM_MACOS_MVK
#include <vulkan/vulkan.hpp>

namespace RHI::Vulkan {

    vk::SurfaceKHR CreateNativeSurface(const vk::Instance& instance, const SwapChainCreateInfo* createInfo)
    {
        auto nsWin = static_cast<NSWindow*>(createInfo->window);

        NSBundle* bundle = [NSBundle bundleWithPath: @"/System/Library/Frameworks/QuartzCore.framework"];
        CALayer* layer = [[bundle classNamed: @"CAMetalLayer"] layer];
        NSView* view = nsWin.contentView;
        [view setLayer: layer];
        [view setWantsLayer: YES];

        vk::MacOSSurfaceCreateInfoMVK surfaceInfo {};
        surfaceInfo.setPView(view);
        vk::SurfaceKHR surface = VK_NULL_HANDLE;
        Assert(instance.createMacOSSurfaceMVK(&surfaceInfo, nullptr, &surface) == vk::Result::eSuccess);
        return surface;
    }
}
#endif
