//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_VULKANCOMMANDBUFFER_H
#define EXPLOSION_VULKANCOMMANDBUFFER_H

#include <functional>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Vulkan/VulkanGraphicsPipeline.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;
    class CommandEncoder;
    class VulkanSignal;
    class VulkanBuffer;
    class VulkanRenderPass;
    class VulkanFrameBuffer;
    class VulkanGraphicsPipeline;

    using EncodingFunc = std::function<void(CommandEncoder* commandEncoder)>;

    class VulkanCommandBuffer {
    public:
        explicit VulkanCommandBuffer(VulkanDriver& driver);
        ~VulkanCommandBuffer();
        const VkCommandBuffer& GetVkCommandBuffer();
        void EncodeCommands(const EncodingFunc& encodingFunc);
        void SubmitNow();
        void Submit(VulkanSignal* waitSignal, VulkanSignal* notifySignal, const std::vector<PipelineStage>& waitStages);

    private:
        void AllocateCommandBuffer();
        void FreeCommandBuffer();

        VulkanDriver& driver;
        VulkanDevice& device;
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
    };

    class CommandEncoder {
    public:
        struct RenderArea {
            int32_t x;
            int32_t y;
            uint32_t width;
            uint32_t height;
        };

        struct ClearValue {
            float r;
            float g;
            float b;
            float a;
        };

        struct RenderPassBeginInfo {
            VulkanFrameBuffer* frameBuffer;
            RenderArea renderArea;
            ClearValue clearValue;
        };

        CommandEncoder(VulkanDriver& driver, VulkanCommandBuffer* commandBuffer);
        ~CommandEncoder();
        void CopyBuffer(VulkanBuffer* srcBuffer, VulkanBuffer* dstBuffer);
        void BeginRenderPass(VulkanRenderPass* renderPass, const RenderPassBeginInfo& renderPassBeginInfo);
        void EndRenderPass();
        void BindGraphicsPipeline(VulkanGraphicsPipeline* pipeline);
        void BindVertexBuffer(uint32_t binding, VulkanBuffer* vertexBuffer);
        void BindIndexBuffer(VulkanBuffer* indexBuffer);
        void Draw(uint32_t firstVertex, uint32_t vertexCount, uint32_t firstInstance, uint32_t instanceCount);
        void DrawIndexed(uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset, uint32_t firstInstance, uint32_t instanceCount);
        void SetViewPort(const VulkanGraphicsPipeline::Viewport& viewport);
        void SetScissor(const VulkanGraphicsPipeline::Scissor& scissor);

    private:
        VulkanDriver& driver;
        VulkanDevice& device;
        VulkanCommandBuffer* commandBuffer = nullptr;
    };
}

#endif //EXPLOSION_VULKANCOMMANDBUFFER_H
