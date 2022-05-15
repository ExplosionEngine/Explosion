//
// Created by Zach Lee on 2022/4/4.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace RHI {
    struct SwapChainCreateInfo;
}

namespace RHI::Vulkan {

    vk::SurfaceKHR CreateNativeSurface(const vk::Instance& instance, const SwapChainCreateInfo* createInfo);
}
