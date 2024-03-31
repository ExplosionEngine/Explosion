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
    VulkanBindGroup::VulkanBindGroup(VulkanDevice& inDevice, const BindGroupCreateInfo& inCreateInfo)
        : BindGroup(inCreateInfo), device(inDevice)
    {
        CreateNativeDescriptorPool(inCreateInfo);
        CreateNativeDescriptorSet(inCreateInfo);
    }

    VulkanBindGroup::~VulkanBindGroup() noexcept
    {
        if (nativeDescriptorPool) {
            vkDestroyDescriptorPool(device.GetNative(), nativeDescriptorPool, nullptr);
        }
    }

    void VulkanBindGroup::Destroy()
    {
        delete this;
    }

    VkDescriptorSet VulkanBindGroup::GetNative() const
    {
        return nativeDescriptorSet;
    }

    void VulkanBindGroup::CreateNativeDescriptorPool(const BindGroupCreateInfo& inCreateInfo)
    {
        const auto entryCount = inCreateInfo.entries.size();

        std::vector<VkDescriptorPoolSize> poolSizes(entryCount);
        for (auto i = 0; i < entryCount; i++) {
            const auto& entry = inCreateInfo.entries[i];

            poolSizes[i].type = VKEnumCast<BindingType, VkDescriptorType>(entry.binding.type);
            poolSizes[i].descriptorCount = 1;
        }

        VkDescriptorPoolCreateInfo poolInfo {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.poolSizeCount = poolSizes.size();
        poolInfo.maxSets = 1;

        Assert(vkCreateDescriptorPool(device.GetNative(), &poolInfo, nullptr, &nativeDescriptorPool) == VK_SUCCESS);
    }

    void VulkanBindGroup::CreateNativeDescriptorSet(const BindGroupCreateInfo& inCreateInfo)
    {
        VkDescriptorSetLayout layout = static_cast<VulkanBindGroupLayout*>(inCreateInfo.layout)->GetNative();

        VkDescriptorSetAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;
        allocInfo.descriptorPool = nativeDescriptorPool;

        Assert(vkAllocateDescriptorSets(device.GetNative(), &allocInfo, &nativeDescriptorSet) == VK_SUCCESS);

#if BUILD_CONFIG_DEBUG
        if (!inCreateInfo.debugName.empty()) {
            device.SetObjectName(VK_OBJECT_TYPE_DESCRIPTOR_SET, reinterpret_cast<uint64_t>(nativeDescriptorSet), inCreateInfo.debugName.c_str());
        }
#endif

        const auto entryCount = inCreateInfo.entries.size();

        std::vector<VkWriteDescriptorSet> descriptorWrites(entryCount);
        std::vector<VkDescriptorImageInfo> imageInfos;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        
        int imageInfosNum = 0;
        int bufferInfosNum = 0;
        for (int i = 0; i < entryCount; i++) {
            const auto& entry = inCreateInfo.entries[i];
            if (entry.binding.type == BindingType::uniformBuffer) {
                bufferInfosNum++;
            } else if (entry.binding.type == BindingType::sampler || entry.binding.type == BindingType::texture) {
                imageInfosNum++;
            }
        }
        imageInfos.reserve(imageInfosNum);
        bufferInfos.reserve(bufferInfosNum);
        
        for (int i = 0; i < entryCount; i++) {
            const auto& entry = inCreateInfo.entries[i];

            descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[i].dstSet = nativeDescriptorSet;
            descriptorWrites[i].dstBinding = std::get<GlslBinding>(entry.binding.platformBinding).index;
            descriptorWrites[i].descriptorCount = 1;
            descriptorWrites[i].descriptorType = VKEnumCast<BindingType, VkDescriptorType>(entry.binding.type);

            if (entry.binding.type == BindingType::uniformBuffer) {
                auto* bufferView = static_cast<VulkanBufferView*>(std::get<BufferView*>(entry.entity));

                bufferInfos.emplace_back();
                bufferInfos.back().buffer = bufferView->GetBuffer().GetNative();
                bufferInfos.back().offset = bufferView->GetOffset();
                bufferInfos.back().range = bufferView->GetBufferSize();

                descriptorWrites[i].pBufferInfo = &bufferInfos.back();
            } else if (entry.binding.type == BindingType::sampler) {
                auto* sampler = static_cast<VulkanSampler*>(std::get<Sampler*>(entry.entity));

                imageInfos.emplace_back();
                imageInfos.back().sampler = sampler->GetNative();

                descriptorWrites[i].pImageInfo = &imageInfos.back();
            } else if (entry.binding.type == BindingType::texture) {
                auto* textureView = static_cast<VulkanTextureView*>(std::get<TextureView*>(entry.entity));

                imageInfos.emplace_back();
                imageInfos.back().imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfos.back().imageView = textureView->GetNative();

                descriptorWrites[i].pImageInfo = &imageInfos.back();
            } else {
                //TODO
            }
        }
        vkUpdateDescriptorSets(device.GetNative(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}
