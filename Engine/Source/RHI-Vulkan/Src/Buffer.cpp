//
// Created by johnk on 2022/1/26.
//

#include <RHI/Vulkan/Buffer.h>
#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/BufferView.h>
#include <RHI/Queue.h>
#include <RHI/CommandBuffer.h>
#include <RHI/CommandRecorder.h>
#include <RHI/Synchronous.h>

namespace RHI::Vulkan {
    VulkanBuffer::VulkanBuffer(VulkanDevice& inDevice, const BufferCreateInfo& inCreateInfo)
        : Buffer(inCreateInfo)
        , device(inDevice)
        , usages(inCreateInfo.usages)
    {
        CreateNativeBuffer(inCreateInfo);
        TransitionToInitState(inCreateInfo);
    }

    VulkanBuffer::~VulkanBuffer()
    {
        if (nativeBuffer != VK_NULL_HANDLE) {
            vmaDestroyBuffer(device.GetNativeAllocator(), nativeBuffer, nativeAllocation);
        }
    }

    void* VulkanBuffer::Map(MapMode inMapMode, size_t inOffset, size_t inLength)
    {
        void* data;
        Assert(vmaMapMemory(device.GetNativeAllocator(), nativeAllocation, &data) == VK_SUCCESS);
        return data;
    }

    void VulkanBuffer::UnMap()
    {
        vmaUnmapMemory(device.GetNativeAllocator(), nativeAllocation);
    }

    Common::UniqueRef<BufferView> VulkanBuffer::CreateBufferView(const BufferViewCreateInfo& inCreateInfo)
    {
        return Common::UniqueRef<BufferView>(new VulkanBufferView(*this, inCreateInfo));
    }

    void VulkanBuffer::CreateNativeBuffer(const BufferCreateInfo& inCreateInfo)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.usage = FlagsCast<BufferUsageFlags, VkBufferUsageFlags>(inCreateInfo.usages);
        bufferInfo.size = inCreateInfo.size;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        if (inCreateInfo.usages | BufferUsageBits::mapWrite) {
            allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        }

        Assert(vmaCreateBuffer(device.GetNativeAllocator(), &bufferInfo, &allocInfo, &nativeBuffer, &nativeAllocation, nullptr) == VK_SUCCESS);

#if BUILD_CONFIG_DEBUG
        if (!inCreateInfo.debugName.empty()) {
            device.SetObjectName(VK_OBJECT_TYPE_BUFFER, reinterpret_cast<uint64_t>(nativeBuffer), inCreateInfo.debugName.c_str());
        }
#endif
    }

    void VulkanBuffer::TransitionToInitState(const BufferCreateInfo& inCreateInfo)
    {
        if (inCreateInfo.initialState > BufferState::undefined) {
            Queue* queue = device.GetQueue(QueueType::graphics, 0);
            Assert(queue);

            Common::UniqueRef<Fence> fence = device.CreateFence(false);
            Common::UniqueRef<CommandBuffer> commandBuffer = device.CreateCommandBuffer();
            Common::UniqueRef<CommandRecorder> commandRecorder = commandBuffer->Begin();
            commandRecorder->ResourceBarrier(Barrier::Transition(this, BufferState::undefined, inCreateInfo.initialState));
            commandRecorder->End();

            QueueSubmitInfo submitInfo {};
            submitInfo.signalFence = fence.Get();
            queue->Submit(commandBuffer.Get(), submitInfo);
            fence->Wait();
        }
    }

    VkBuffer VulkanBuffer::GetNative()
    {
        return nativeBuffer;
    }

    BufferUsageFlags VulkanBuffer::GetUsages()
    {
        return usages;
    }
}
