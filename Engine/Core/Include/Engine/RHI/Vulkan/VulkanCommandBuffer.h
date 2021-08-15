//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_VULKANCOMMANDBUFFER_H
#define EXPLOSION_VULKANCOMMANDBUFFER_H

#include <vulkan/vulkan.h>

#include <Engine/RHI/Common/CommandBuffer.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;
    class VulkanSignal;
    class VulkanBuffer;
    class VulkanRenderPass;
    class VulkanFrameBuffer;
    class VulkanGraphicsPipeline;

    class VulkanCommandBuffer : public CommandBuffer {
    public:
        explicit VulkanCommandBuffer(VulkanDriver& driver);
        ~VulkanCommandBuffer() override;
        void EncodeCommands(const EncodingFunc& encodingFunc) override;
        void SubmitNow() override;
        void Submit(Signal* waitSignal, Signal* notifySignal, PipelineStageFlags waitStages) override;

        const VkCommandBuffer& GetVkCommandBuffer();

    private:
        void AllocateCommandBuffer();
        void FreeCommandBuffer();

        VulkanDriver& driver;
        VulkanDevice& device;
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
    };

    class VulkanCommandEncoder : public CommandEncoder {
    public:
        VulkanCommandEncoder(VulkanDriver& driver, VulkanCommandBuffer* commandBuffer);
        ~VulkanCommandEncoder() override;

        void CopyBuffer(Buffer* srcBuffer, Buffer* dstBuffer) override;
        void BeginRenderPass(RenderPass* renderPass, const RenderPassBeginInfo& renderPassBeginInfo) override;
        void EndRenderPass() override;
        void BindGraphicsPipeline(GraphicsPipeline* pipeline) override;
        void BindVertexBuffer(uint32_t binding, Buffer* vertexBuffer) override;
        void BindIndexBuffer(Buffer* indexBuffer) override;
        void Draw(uint32_t firstVertex, uint32_t vertexCount, uint32_t firstInstance, uint32_t instanceCount) override;
        void DrawIndexed(uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset, uint32_t firstInstance, uint32_t instanceCount) override;
        void SetViewPort(const GraphicsPipeline::Viewport& viewport) override;
        void SetScissor(const GraphicsPipeline::Scissor& scissor) override;
        void BindDescriptorSet(const std::vector<DescriptorSet*>& descriptorSets) override;

    private:
        VulkanDriver& driver;
        VulkanDevice& device;
        VulkanCommandBuffer* commandBuffer = nullptr;
    };
}

#endif //EXPLOSION_VULKANCOMMANDBUFFER_H
