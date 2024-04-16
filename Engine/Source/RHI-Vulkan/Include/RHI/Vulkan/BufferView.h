//
// Created by swtpotato on 2022/8/2.
//

#pragma once

#include <RHI/BufferView.h>

namespace RHI::Vulkan {
    class VulkanBuffer;
    class VulkanDevice;

    class VulkanBufferView : public BufferView {
    public:
        NonCopyable(VulkanBufferView)
        VulkanBufferView(VulkanBuffer& inBuffer, const BufferViewCreateInfo& inCreateInfo);
        ~VulkanBufferView() override;

        size_t GetOffset() const;
        size_t GetBufferSize() const;
        IndexFormat GetIndexFormat() const;
        VulkanBuffer& GetBuffer();

    private:
        void InitializeBufferAttrib(const BufferViewCreateInfo& inCreateInfo);

        VulkanBuffer& buffer;
        size_t size;
        size_t offset;
        IndexFormat format;
    };
}
