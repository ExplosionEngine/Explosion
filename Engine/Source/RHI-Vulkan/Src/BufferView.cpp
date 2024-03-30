//
// Created by swtpotato on 2022/8/2.
//

#include <utility>

#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Buffer.h>
#include <RHI/Vulkan/BufferView.h>

namespace RHI::Vulkan {
    static inline bool IsVertexBuffer(BufferUsageFlags bufferUsages)
    {
        return (bufferUsages & BufferUsageBits::vertex) != 0;
    }

    static inline bool IsIndexBuffer(BufferUsageFlags bufferUsages)
    {
        return (bufferUsages & BufferUsageBits::index) != 0;
    }
}

namespace RHI::Vulkan {
    VulkanBufferView::VulkanBufferView(VulkanBuffer& inBuffer, const BufferViewCreateInfo& inCreateInfo)
        : BufferView(inCreateInfo), buffer(inBuffer)
    {
        InitializeBufferAttrib(inCreateInfo);
    }

    VulkanBufferView::~VulkanBufferView()=default;

    void VulkanBufferView::Destroy()
    {
        delete this;
    }

    void VulkanBufferView::InitializeBufferAttrib(const BufferViewCreateInfo& inCreateInfo)
    {
        offset = inCreateInfo.offset;
        size = inCreateInfo.size;
        if (IsIndexBuffer(buffer.GetUsages())) {
            format = std::get<IndexBufferViewInfo>(inCreateInfo.extend).format;
        } else {
            // TODO
            // Uniform buffer
        }
    }

    size_t VulkanBufferView::GetBufferSize() const
    {
        return size;
    }

    size_t VulkanBufferView::GetOffset() const
    {
        return offset;
    }

    IndexFormat VulkanBufferView::GetIndexFormat() const
    {
        return format;
    }

    VulkanBuffer& VulkanBufferView::GetBuffer()
    {
        return buffer;
    }
}