//
// Created by Zach Lee on 2022/3/7.
//


#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>
#include <RHI/TextureView.h>

namespace RHI::Vulkan {
    class VKDevice;

    class VKTextureView : public TextureView {
    public:
        NON_COPYABLE(VKTextureView)
        VKTextureView(VKDevice& device, const TextureViewCreateInfo* createInfo);
        ~VKTextureView() override;

        void Destroy() override;

    private:
    };
}