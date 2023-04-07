//
// Created by Zach Lee on 2022/3/20.
//

#include <RHI/Vulkan/BindGroup.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/BindGroupLayout.h>
#include <RHI/Vulkan/Buffer.h>
#include <RHI/Vulkan/BufferView.h>
#include <RHI/Vulkan/Sampler.h>
#include <RHI/Vulkan/TextureView.h>
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {
    VKBindGroup::VKBindGroup(VKDevice& device, const BindGroupCreateInfo& createInfo)
        : BindGroup(createInfo), device(device)
    {
        CreateDescriptorPool(createInfo);
        CreateDescriptorSet(createInfo);
    }

    VKBindGroup::~VKBindGroup() noexcept
    {
        if (descriptorPool) {
            device.GetVkDevice().destroyDescriptorPool(descriptorPool, nullptr);
        }
    }

    void VKBindGroup::Destroy()
    {
        delete this;
    }

    vk::DescriptorSet VKBindGroup::GetVkDescritorSet() const
    {
        return descriptorSet;
    }

    void VKBindGroup::CreateDescriptorPool(const BindGroupCreateInfo& createInfo)
    {
        std::vector<vk::DescriptorPoolSize> poolSizes(createInfo.entryNum);

        for (auto i = 0; i < createInfo.entryNum; i++) {
            const auto& entry = createInfo.entries[i];

            poolSizes[i].setType(VKEnumCast<BindingType, vk::DescriptorType>(entry.type))
                .setDescriptorCount(1);
        }

        vk::DescriptorPoolCreateInfo poolInfo {};
        poolInfo.setPPoolSizes(poolSizes.data())
            .setPoolSizeCount(createInfo.entryNum)
            .setMaxSets(1);

        Assert(device.GetVkDevice().createDescriptorPool(&poolInfo, nullptr, &descriptorPool) == vk::Result::eSuccess);
    }

    void VKBindGroup::CreateDescriptorSet(const BindGroupCreateInfo& createInfo)
    {
        vk::DescriptorSetLayout layout = dynamic_cast<VKBindGroupLayout*>(createInfo.layout)->GetVkDescriptorSetLayout();

        vk::DescriptorSetAllocateInfo allocInfo {};
        allocInfo.setDescriptorSetCount(1)
            .setPSetLayouts(&layout)
            .setDescriptorPool(descriptorPool);

        Assert(device.GetVkDevice().allocateDescriptorSets(&allocInfo, &descriptorSet) == vk::Result::eSuccess);

        std::vector<vk::WriteDescriptorSet> descriptorWrites(createInfo.entryNum);
        for (int i = 0; i < createInfo.entryNum; i++) {
            const auto& entry = createInfo.entries[i];

            descriptorWrites[i].setDstSet(descriptorSet)
                .setDstBinding(entry.binding.glsl.index)
                .setDescriptorCount(1)
                .setDescriptorType(VKEnumCast<BindingType, vk::DescriptorType>(entry.type));

            if (entry.type == BindingType::UNIFORM_BUFFER) {
                auto* bufferView = dynamic_cast<VKBufferView*>(entry.bufferView);

                vk::DescriptorBufferInfo bufferInfo;
                bufferInfo.setBuffer(bufferView->GetBuffer().GetVkBuffer())
                    .setOffset(bufferView->GetOffset())
                    .setRange(bufferView->GetBufferSize());

                descriptorWrites[i].setBufferInfo(bufferInfo);
            } else if (entry.type == BindingType::SAMPLER) {
                auto* sampler = dynamic_cast<VKSampler*>(entry.sampler);

                vk::DescriptorImageInfo samplerInfo {};
                samplerInfo.setSampler(sampler->GetVkSampler());

                descriptorWrites[i].setImageInfo(samplerInfo);
            } else if (entry.type == BindingType::TEXTURE) {
                auto* textureView = dynamic_cast<VKTextureView*>(entry.textureView);

                vk::DescriptorImageInfo imageInfo{};
                imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                    .setImageView(textureView->GetVkImageView());

                descriptorWrites[i].setImageInfo(imageInfo);
            } else {
                //TODO
            }
        }
        device.GetVkDevice().updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}
