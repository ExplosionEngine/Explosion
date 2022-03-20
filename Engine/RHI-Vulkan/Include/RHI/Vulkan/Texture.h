//
// Created by Zach Lee on 2022/3/7.
//


#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>
#include <RHI/Texture.h>

namespace RHI::Vulkan {
    class VKDevice;

    class VKTexture : public Texture {
    public:
        NON_COPYABLE(VKTexture)
        VKTexture(VKDevice& device, const TextureCreateInfo* createInfo);
        ~VKTexture() override;

        void Destroy() override;

    private:
    };
}