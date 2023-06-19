//
// Created by Zach Lee on 2022/3/20.
//


#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#include <RHI/BindGroup.h>

namespace RHI::Vulkan {
    class VKDevice;

    class VKBindGroup : public BindGroup {
    public:
        NON_COPYABLE(VKBindGroup)
        VKBindGroup(VKDevice& device, const BindGroupCreateInfo& createInfo);
        ~VKBindGroup() noexcept override;

        void Destroy() override;

        VkDescriptorSet GetVkDescritorSet() const;
    private:
        void CreateDescriptorPool(const BindGroupCreateInfo& createInfo);
        void CreateDescriptorSet(const BindGroupCreateInfo& createInfo);
        VKDevice& device;
        VkDescriptorSet descriptorSet;
        VkDescriptorPool descriptorPool;
    };
}