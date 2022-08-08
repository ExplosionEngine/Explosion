//
// Created by Zach Lee on 2022/6/4.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <RHI/CommandBuffer.h>

namespace RHI::Vulkan {

    class VKDevice;

    class VKCommandBuffer : public CommandBuffer {
    public:
        NON_COPYABLE(VKCommandBuffer)
        VKCommandBuffer(VKDevice& device, vk::CommandPool);
        ~VKCommandBuffer() override;

        void Destroy() override;

        CommandEncoder* Begin() override;

        vk::CommandBuffer GetVkCommandBuffer() const;
    private:
        void CreateNativeCommandBuffer();

        VKDevice& device;
        vk::CommandPool pool;
        vk::CommandBuffer commandBuffer;
    };

}
