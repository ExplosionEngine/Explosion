//
// Created by Zach Lee on 2022/3/6.
//

#include <RHI/Vulkan/BindGroupLayout.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>
#include <vector>

namespace RHI::Vulkan {
    static vk::DescriptorType GetVKDescriptorType(BindingType type)
    {
        static std::unordered_map<BindingType, vk::DescriptorType> types = {
            { BindingType::UNIFORM_BUFFER,  vk::DescriptorType::eUniformBuffer },
            { BindingType::STORAGE_BUFFER,  vk::DescriptorType::eStorageBuffer },
            { BindingType::SAMPLER,         vk::DescriptorType::eCombinedImageSampler },
            { BindingType::TEXTURE,         vk::DescriptorType::eSampledImage },
            { BindingType::STORAGE_TEXTURE, vk::DescriptorType::eStorageImage }
        };

        auto iter = types.find(type);
        if (iter == types.end()) {
            throw VKException("BindType not supported.");
        }
        return iter->second;
    }

    static vk::ShaderStageFlags GetShaderStageFlags(ShaderStageFlags flag)
    {
        static std::unordered_map<ShaderStageBits, vk::ShaderStageFlagBits> stageBits = {
            { ShaderStageBits::VERTEX, vk::ShaderStageFlagBits::eVertex },
            { ShaderStageBits::FRAGMENT, vk::ShaderStageFlagBits::eFragment },
            { ShaderStageBits::COMPUTE, vk::ShaderStageFlagBits::eCompute }
        };
        vk::ShaderStageFlags result = {};
        for (const auto& stage : stageBits) {
            if (flag & stage.first) {
                result |= stage.second;
            }
        }
        return result;
    }

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

    void VKBindGroupLayout::CreateDescriptorSetLayout(const BindGroupLayoutCreateInfo* createInfo)
    {
        vk::DescriptorSetLayoutCreateInfo layoutInfo = {};

        std::vector<vk::DescriptorSetLayoutBinding> bindings(createInfo->entryNum);
        for (size_t i = 0; i < createInfo->entryNum; ++i) {
            auto& entry = createInfo->entries[i];
            auto& binding = bindings[i];
            binding.setDescriptorType(GetVKDescriptorType(entry.type))
                .setDescriptorCount(1)
                .setBinding(entry.binding)
                .setStageFlags(GetShaderStageFlags(entry.shaderVisibility));
        }

        layoutInfo.setBindings(bindings);

        if (device.GetVkDevice().createDescriptorSetLayout(&layoutInfo, nullptr, &setLayout) != vk::Result::eSuccess) {
            throw VKException("failed to create descriptorSetLayout");
        }
    }

}