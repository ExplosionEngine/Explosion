//
// Created by swtpotato on 2022/8/2.
//

#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Buffer.h>
#include <RHI/Vulkan/BufferView.h>

namespace RHI::Vulkan {
    static inline bool IsVertexBuffer(BufferUsageFlags bufferUsages)
    {
        return (bufferUsages & BufferUsageBits::VERTEX) != 0;
    }

    static inline bool IsIndexBuffer(BufferUsageFlags bufferUsages)
    {
        return (bufferUsages & BufferUsageBits::INDEX) != 0;
    }
}

namespace RHI::Vulkan {
    VKBufferView::VKBufferView(VKBuffer& buffer, const BufferViewCreateInfo& createInfo)
        :BufferView(createInfo), buffer(buffer)
    {
        InitializeBufferAttrib(createInfo);
    }

    VKBufferView::~VKBufferView()=default;

    void VKBufferView::Destroy()
    {
        delete this;
    }

    void VKBufferView::InitializeBufferAttrib(const BufferViewCreateInfo& createInfo)
    {
        offset = createInfo.offset;
        if (IsIndexBuffer(buffer.GetUsages())) {
            format = createInfo.index.format;
        } else {
            // TODO
            // Uniform buffer
        }
    }

    size_t VKBufferView::GetBufferSize() const
    {
        return size;
    }

    size_t VKBufferView::GetOffset() const
    {
        return offset;
    }

    IndexFormat VKBufferView::GetIndexFormat() const
    {
        return format;
    }

    VKBuffer& VKBufferView::GetBuffer()
    {
        return buffer;
    }
}