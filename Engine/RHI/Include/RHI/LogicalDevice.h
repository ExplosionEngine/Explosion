//
// Created by johnk on 31/12/2021.
//

#ifndef EXPLOSION_RHI_LOGICAL_DEVICE_H
#define EXPLOSION_RHI_LOGICAL_DEVICE_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    class Queue;
    class SwapChain;
    class DeviceMemory;
    class Buffer;
    class RenderPass;
    class FrameBuffer;
    class Pipeline;
    class DescriptorSetLayout;
    class PipelineLayout;
    struct QueueFamilyCreateInfo;
    struct SwapChainCreateInfo;
    struct DeviceMemoryAllocateInfo;
    struct BufferCreateInfo;
    struct RenderPassCreateInfo;
    struct FrameBufferCreateInfo;
    struct GraphicsPipelineCreateInfo;
    struct DescriptorSetLayoutCreateInfo;
    struct PipelineLayoutCreateInfo;

    struct LogicalDeviceCreateInfo {
        size_t queueFamilyNum;
        const QueueFamilyCreateInfo* queueFamilyCreateInfos;
        size_t extensionNum;
        const char** extensions;
    };

    class LogicalDevice {
    public:
        NON_COPYABLE(LogicalDevice)
        virtual ~LogicalDevice();

        virtual size_t GetQueueNum(QueueFamilyType familyType) = 0;
        virtual Queue* GetCommandQueue(QueueFamilyType familyType, size_t idx) = 0;
        virtual SwapChain* CreateSwapChain(const SwapChainCreateInfo* createInfo) = 0;
        virtual void DestroySwapChain(SwapChain* swapChain) = 0;
        virtual DeviceMemory* AllocateDeviceMemory(const DeviceMemoryAllocateInfo* createInfo) = 0;
        virtual void FreeDeviceMemory(DeviceMemory* deviceMemory) = 0;
        virtual Buffer* CreateBuffer(const BufferCreateInfo* createInfo) = 0;
        virtual void DestroyBuffer(Buffer* buffer) = 0;
        virtual void BindBufferMemory(Buffer* buffer, DeviceMemory* deviceMemory) = 0;
        virtual void* MapDeviceMemory(DeviceMemory* deviceMemory) = 0;
        virtual void UnmapDeviceMemory(DeviceMemory* deviceMemory) = 0;
        virtual RenderPass* CreateRenderPass(const RenderPassCreateInfo* createInfo) = 0;
        virtual void DestroyRenderPass(RenderPass* renderPass) = 0;
        virtual FrameBuffer* CreateFrameBuffer(const FrameBufferCreateInfo* createInfo) = 0;
        virtual void DestroyFrameBuffer(FrameBuffer* frameBuffer) = 0;
        virtual DescriptorSetLayout* CreateDescriptorSetLayout(const DescriptorSetLayoutCreateInfo* createInfo) = 0;
        virtual void DestroyDescriptorSetLayout(DescriptorSetLayout* descriptorSetLayout) = 0;
        virtual PipelineLayout* CreatePipelineLayout(const PipelineLayoutCreateInfo* createInfo) = 0;
        virtual void DestroyPipelineLayout(PipelineLayout* pipelineLayout) = 0;
        virtual Pipeline* CreateGraphicsPipeline(const GraphicsPipelineCreateInfo* createInfo) = 0;
        virtual void DestroyPipeline(Pipeline* pipeline) = 0;

    protected:
        explicit LogicalDevice(const LogicalDeviceCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_LOGICAL_DEVICE_H
