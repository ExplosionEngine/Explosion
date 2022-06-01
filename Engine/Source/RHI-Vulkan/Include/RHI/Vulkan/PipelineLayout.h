//
// Created by Zach Lee on 2022/4/2.
//

#pragma once
#include <RHI/PipelineLayout.h>
#include <vulkan/vulkan.hpp>

namespace RHI::Vulkan {
    class VKDevice;

    class VKPipelineLayout : public PipelineLayout {
    public:
        NON_COPYABLE(VKPipelineLayout)
        VKPipelineLayout(VKDevice& device, const PipelineLayoutCreateInfo* createInfo);
        ~VKPipelineLayout() override;

        void Destroy() override;

        vk::PipelineLayout GetNativeHandle() const;
    private:
        void CreateNativePipelineLayout(const PipelineLayoutCreateInfo* createInfo);

        VKDevice& device;
        vk::PipelineLayout pipelineLayout = VK_NULL_HANDLE;
    };


}