//
// Created by johnk on 15/1/2022.
//

#ifndef EXPLOSION_RHI_DEVICE_H
#define EXPLOSION_RHI_DEVICE_H

#include <cstdint>

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    struct BufferCreateInfo;
    struct TextureCreateInfo;
    struct SamplerCreateInfo;
    struct BindGroupLayoutCreateInfo;
    struct BindGroupCreateInfo;
    struct PipelineLayoutCreateInfo;
    struct ShaderModuleCreateInfo;
    struct ComputePipelineCreateInfo;
    struct GraphicsPipelineCreateInfo;
    struct CommandBufferCreateInfo;
    class Queue;
    class Buffer;
    class Texture;
    class Sampler;
    class BindGroupLayout;
    class BindGroup;
    class PipelineLayout;
    class ShaderModule;
    class ComputePipeline;
    class GraphicsPipeline;
    class CommandBuffer;

    struct QueueInfo {
        QueueType type;
        uint8_t num;
    };

    struct DeviceCreateInfo {
        uint32_t queueCreateInfoNum;
        const QueueInfo* queueCreateInfos;
    };

    class Device {
    public:
        NON_COPYABLE(Device)
        virtual ~Device();

        virtual void Destroy() = 0;
        virtual size_t GetQueueNum(QueueType type) = 0;
        virtual Queue* GetQueue(QueueType type, size_t index) = 0;
        virtual Buffer* CreateBuffer(const BufferCreateInfo* createInfo) = 0;
        virtual Texture* CreateTexture(const TextureCreateInfo* createInfo) = 0;
        virtual Sampler* CreateSampler(const SamplerCreateInfo* createInfo) = 0;
        virtual BindGroupLayout* CreateBindGroupLayout(const BindGroupLayoutCreateInfo* createInfo) = 0;
        virtual BindGroup* CreateBindGroup(const BindGroupCreateInfo* createInfo) = 0;
        virtual PipelineLayout* CreatePipelineLayout(const PipelineLayoutCreateInfo* createInfo) = 0;
        virtual ShaderModule* CreateShaderModule(const ShaderModuleCreateInfo* createInfo) = 0;
        virtual ComputePipeline* CreateComputePipeline(const ComputePipelineCreateInfo* createInfo) = 0;
        virtual GraphicsPipeline* CreateGraphicsPipeline(const GraphicsPipelineCreateInfo* createInfo) = 0;
        virtual CommandBuffer* CreateCommandBuffer(const CommandBufferCreateInfo* createInfo) = 0;

    protected:
        explicit Device(const DeviceCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_DEVICE_H
