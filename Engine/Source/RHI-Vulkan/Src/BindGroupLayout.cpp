//
// Created by Zach Lee on 2022/3/6.
//

#include <RHI/Vulkan/BindGroupLayout.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>
#include <vector>

namespace RHI::Vulkan {
    VulkanBindGroupLayout::VulkanBindGroupLayout(VulkanDevice& inDevice, const BindGroupLayoutCreateInfo& inCreateInfo)
        : BindGroupLayout(inCreateInfo)
        , device(inDevice)
    {
        CreateNativeDescriptorSetLayout(inCreateInfo);
    }

    VulkanBindGroupLayout::~VulkanBindGroupLayout()
    {
        if (nativeDescriptorSetLayout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(device.GetNative(), nativeDescriptorSetLayout, nullptr);
        }
    }

    VkDescriptorSetLayout VulkanBindGroupLayout::GetNative() const
    {
        return nativeDescriptorSetLayout;
    }

    void VulkanBindGroupLayout::CreateNativeDescriptorSetLayout(const BindGroupLayoutCreateInfo& inCreateInfo)
    {
        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

        std::vector<VkDescriptorSetLayoutBinding> bindings(inCreateInfo.entries.size());
        for (size_t i = 0; i < bindings.size(); ++i) {
            const auto& entry = inCreateInfo.entries[i];
            auto& binding = bindings[i];

            VkShaderStageFlags flags = VKFlagsCast<ShaderStageFlags, VkShaderStageFlags>(entry.shaderVisibility);

            binding.descriptorType = VKEnumCast<BindingType, VkDescriptorType>(entry.binding.type);
            binding.descriptorCount = 1;
            binding.binding = std::get<GlslBinding>(entry.binding.platformBinding).index;
            binding.stageFlags = flags;
        }

        layoutInfo.pBindings = bindings.data();
        layoutInfo.bindingCount = bindings.size();

        Assert(vkCreateDescriptorSetLayout(device.GetNative(), &layoutInfo, nullptr, &nativeDescriptorSetLayout) == VK_SUCCESS);

#if BUILD_CONFIG_DEBUG
        if (!inCreateInfo.debugName.empty()) {
            device.SetObjectName(VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, reinterpret_cast<uint64_t>(nativeDescriptorSetLayout), inCreateInfo.debugName.c_str());
        }
#endif
    }
}