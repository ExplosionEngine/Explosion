//
// Created by Zach Lee on 2022/3/6.
//

#include <RHI/Vulkan/BindGroupLayout.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>
#include <vector>

namespace RHI::Vulkan {
    VKBindGroupLayout::VKBindGroupLayout(VKDevice& dev, const BindGroupLayoutCreateInfo* createInfo)
        : BindGroupLayout(createInfo), device(dev)
    {
        CreateDescriptorSetLayout(createInfo);
    }

    VKBindGroupLayout::~VKBindGroupLayout()
    {
        if (setLayout) {
            device.GetVkDevice().destroyDescriptorSetLayout(setLayout, nullptr);
        }
    }

    void VKBindGroupLayout::Destroy()
    {
        delete this;
    }

    vk::DescriptorSetLayout VKBindGroupLayout::GetVkDescriptorSetLayout() const
    {
        return setLayout;
    }

    void VKBindGroupLayout::CreateDescriptorSetLayout(const BindGroupLayoutCreateInfo* createInfo)
    {
        vk::DescriptorSetLayoutCreateInfo layoutInfo = {};

        std::vector<vk::DescriptorSetLayoutBinding> bindings(createInfo->entryNum);
        for (size_t i = 0; i < createInfo->entryNum; ++i) {
            auto& entry = createInfo->entries[i];
            auto& binding = bindings[i];

            vk::ShaderStageFlags flags = FromRHI(entry.shaderVisibility);

            binding.setDescriptorType(VKEnumCast<BindingType, vk::DescriptorType>(entry.type))
                .setDescriptorCount(1)
                .setBinding(entry.glslBinding)
                .setStageFlags(flags);
        }

        layoutInfo.setBindings(bindings);

        Assert(device.GetVkDevice().createDescriptorSetLayout(&layoutInfo, nullptr, &setLayout) == vk::Result::eSuccess);
    }

}