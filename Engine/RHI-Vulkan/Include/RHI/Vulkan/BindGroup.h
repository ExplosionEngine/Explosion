//
// Created by Zach Lee on 2022/3/20.
//


#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>
#include <RHI/BindGroup.h>

namespace RHI::Vulkan {
    class VKDevice;

    class VKBindGroup : public BindGroup {
    public:
        NON_COPYABLE(VKBindGroup)
        VKBindGroup(VKDevice& device, const BindGroupLayoutCreateInfo* createInfo);
        ~VKBindGroup() override;

        void Destroy() override;

    private:
        vk::DescriptorSetLayout setLayout;
    };
}