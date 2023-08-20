//
// Created by swtpotato on 2022/8/2.
//

#pragma once

#include <RHI/BufferView.h>

namespace RHI::Vulkan {
    class VKBuffer;
    class VKDevice;

    class VKBufferView : public BufferView {
    public:
        NonCopyable(VKBufferView)
        VKBufferView(VKBuffer& buffer, const BufferViewCreateInfo& createInfo);
        ~VKBufferView() override;
        void Destroy() override;

        size_t GetOffset() const;
        size_t GetBufferSize() const;
        IndexFormat GetIndexFormat() const;
        VKBuffer& GetBuffer();

    private:
        VKBuffer& buffer;
        size_t size;
        size_t offset;
        IndexFormat format;

        void InitializeBufferAttrib(const BufferViewCreateInfo& createInfo);
    };
}
