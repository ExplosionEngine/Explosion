//
// Created by Zach Lee on 2022/6/4.
//

#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <RHI/CommandBuffer.h>

namespace RHI::Vulkan {

    class VKDevice;

    class VKCommandBuffer : public CommandBuffer {
    public:
        NonCopyable(VKCommandBuffer)
        VKCommandBuffer(VKDevice& device, VkCommandPool);
        ~VKCommandBuffer() override;

        void Destroy() override;
        CommandEncoder* Begin() override;

        VkCommandBuffer GetVkCommandBuffer() const;

    private:
        void CreateNativeCommandBuffer();

        VKDevice& device;
        VkCommandPool pool;
        VkCommandBuffer commandBuffer;
    };

}
