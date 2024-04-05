//
// Created by johnk on 15/1/2022.
//

#pragma once

#include <cstdint>

#include <Common/Utility.h>
#include <RHI/Common.h>

#if PLATFORM_WINDOWS
#undef CreateSemaphore
#endif

namespace RHI {
    struct BufferCreateInfo;
    struct TextureCreateInfo;
    struct SamplerCreateInfo;
    struct BindGroupLayoutCreateInfo;
    struct BindGroupCreateInfo;
    struct PipelineLayoutCreateInfo;
    struct ShaderModuleCreateInfo;
    struct ComputePipelineCreateInfo;
    struct RasterPipelineCreateInfo;
    struct SwapChainCreateInfo;
    struct SurfaceCreateInfo;
    class Queue;
    class Buffer;
    class Texture;
    class Sampler;
    class BindGroupLayout;
    class BindGroup;
    class PipelineLayout;
    class ShaderModule;
    class ComputePipeline;
    class RasterPipeline;
    class CommandBuffer;
    class SwapChain;
    class Fence;
    class Surface;
    class Semaphore;

    struct QueueRequestInfo {
        QueueType type;
        uint8_t num;

        QueueRequestInfo(QueueType inType, uint8_t inNum);
    };

    struct DeviceCreateInfo {
        std::vector<QueueRequestInfo> queueRequests;

        DeviceCreateInfo();
        DeviceCreateInfo& AddQueueRequest(const QueueRequestInfo& inQueue);
    };

    class Device {
    public:
        NonCopyable(Device)
        virtual ~Device();

        virtual void Destroy() = 0;
        virtual size_t GetQueueNum(QueueType type) = 0;
        virtual Queue* GetQueue(QueueType type, size_t index) = 0;
        virtual Surface* CreateSurface(const SurfaceCreateInfo& createInfo) = 0;
        virtual SwapChain* CreateSwapChain(const SwapChainCreateInfo& createInfo) = 0;
        virtual Buffer* CreateBuffer(const BufferCreateInfo& createInfo) = 0;
        virtual Texture* CreateTexture(const TextureCreateInfo& createInfo) = 0;
        virtual Sampler* CreateSampler(const SamplerCreateInfo& createInfo) = 0;
        virtual BindGroupLayout* CreateBindGroupLayout(const BindGroupLayoutCreateInfo& createInfo) = 0;
        virtual BindGroup* CreateBindGroup(const BindGroupCreateInfo& createInfo) = 0;
        virtual PipelineLayout* CreatePipelineLayout(const PipelineLayoutCreateInfo& createInfo) = 0;
        virtual ShaderModule* CreateShaderModule(const ShaderModuleCreateInfo& createInfo) = 0;
        virtual ComputePipeline* CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) = 0;
        virtual RasterPipeline* CreateRasterPipeline(const RasterPipelineCreateInfo& createInfo) = 0;
        virtual CommandBuffer* CreateCommandBuffer() = 0;
        virtual Fence* CreateFence(bool bInitAsSignaled) = 0;
        virtual Semaphore* CreateSemaphore() = 0;

        virtual bool CheckSwapChainFormatSupport(Surface* surface, PixelFormat format) = 0;

    protected:
        explicit Device(const DeviceCreateInfo& createInfo);
    };
}
