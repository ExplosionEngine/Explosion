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
        NON_COPYABLE(VKBufferView)
        VKBufferView(VKBuffer& buffer, const BufferViewCreateInfo* createInfo);
        ~VKBufferView() override;
        void Destroy() override;

    private:
        VKBuffer& buffer;

        void CreateVKDescriptor(const BufferViewCreateInfo* createInfo);
    };
}
