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
            vkDestroyDescriptorPool(device.GetVkDevice(), descriptorPool, nullptr);
        }
    }

    void VKBindGroup::Destroy()
    {
        delete this;
    }

    VkDescriptorSet VKBindGroup::GetVkDescritorSet() const
    {
        return descriptorSet;
    }

    void VKBindGroup::CreateDescriptorPool(const BindGroupCreateInfo& createInfo)
    {
        std::vector<VkDescriptorPoolSize> poolSizes(createInfo.entryNum);

        for (auto i = 0; i < createInfo.entryNum; i++) {
            const auto& entry = createInfo.entries[i];

            poolSizes[i].type = VKEnumCast<BindingType, VkDescriptorType>(entry.binding.type);
            poolSizes[i].descriptorCount = 1;
        }

        VkDescriptorPoolCreateInfo poolInfo {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.poolSizeCount = createInfo.entryNum;
        poolInfo.maxSets = 1;

        Assert(vkCreateDescriptorPool(device.GetVkDevice(), &poolInfo, nullptr, &descriptorPool) == VK_SUCCESS);
    }

    void VKBindGroup::CreateDescriptorSet(const BindGroupCreateInfo& createInfo)
    {
        VkDescriptorSetLayout layout = dynamic_cast<VKBindGroupLayout*>(createInfo.layout)->GetVkDescriptorSetLayout();

        VkDescriptorSetAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;
        allocInfo.descriptorPool = descriptorPool;

        Assert(vkAllocateDescriptorSets(device.GetVkDevice(), &allocInfo, &descriptorSet) == VK_SUCCESS);

#if BUILD_CONFIG_DEBUG
        if (!createInfo.debugName.empty()) {
            device.SetObjectName(VK_OBJECT_TYPE_DESCRIPTOR_SET, reinterpret_cast<uint64_t>(descriptorSet), createInfo.debugName.c_str());
        }
#endif

        std::vector<VkWriteDescriptorSet> descriptorWrites(createInfo.entryNum);
        std::vector<VkDescriptorImageInfo> imageInfos;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        
        int imageInfosNum = 0;
        int bufferInfosNum = 0;
        for (int i = 0; i < createInfo.entryNum; i++) {
            const auto& entry = createInfo.entries[i];
            if (entry.binding.type == BindingType::uniformBuffer) {
                bufferInfosNum++;
            } else if (entry.binding.type == BindingType::sampler || entry.binding.type == BindingType::texture) {
                imageInfosNum++;
            }
        }
        imageInfos.reserve(imageInfosNum);
        bufferInfos.reserve(bufferInfosNum);
        
        for (int i = 0; i < createInfo.entryNum; i++) {
            const auto& entry = createInfo.entries[i];

            descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[i].dstSet = descriptorSet;
            descriptorWrites[i].dstBinding = std::get<GlslBinding>(entry.binding.platformBinding).index;
            descriptorWrites[i].descriptorCount = 1;
            descriptorWrites[i].descriptorType = VKEnumCast<BindingType, VkDescriptorType>(entry.binding.type);

            if (entry.binding.type == BindingType::uniformBuffer) {
                auto* bufferView = dynamic_cast<VKBufferView*>(entry.bufferView);

                bufferInfos.emplace_back();
                bufferInfos.back().buffer = bufferView->GetBuffer().GetVkBuffer();
                bufferInfos.back().offset = bufferView->GetOffset();
                bufferInfos.back().range = bufferView->GetBufferSize();

                descriptorWrites[i].pBufferInfo = &bufferInfos.back();
            } else if (entry.binding.type == BindingType::sampler) {
                auto* sampler = dynamic_cast<VKSampler*>(entry.sampler);

                imageInfos.emplace_back();
                imageInfos.back().sampler = sampler->GetVkSampler();

                descriptorWrites[i].pImageInfo = &imageInfos.back();
            } else if (entry.binding.type == BindingType::texture) {
                auto* textureView = dynamic_cast<VKTextureView*>(entry.textureView);

                imageInfos.emplace_back();
                imageInfos.back().imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfos.back().imageView = textureView->GetVkImageView();

                descriptorWrites[i].pImageInfo = &imageInfos.back();
            } else {
                //TODO
            }
        }
        vkUpdateDescriptorSets(device.GetVkDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}
