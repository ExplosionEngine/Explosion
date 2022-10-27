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
        VKBindGroup(VKDevice& device, const BindGroupCreateInfo* createInfo);
        ~VKBindGroup() override;

        void Destroy() override;

        vk::DescriptorSet GetVkDescritorSet() const;
    private:
        void CreateDescriptorPool(const BindGroupCreateInfo* createInfo);
        void CreateDescriptorSet(const BindGroupCreateInfo* createInfo);
        VKDevice& device;
        vk::DescriptorSet descriptorSet;
        vk::DescriptorPool descriptorPool;
    };
}