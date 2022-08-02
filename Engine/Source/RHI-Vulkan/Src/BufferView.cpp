//
// Created by swtpotato on 2022/8/2.
//

#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Buffer.h>
#include <RHI/Vulkan/BufferView.h>

namespace RHI::Vulkan {
    VKBufferView::VKBufferView(VKBuffer& buffer, const BufferViewCreateInfo* createInfo)
        :BufferView(createInfo), buffer(buffer)
    {
        CreateVKDescriptor(createInfo);
    }

    VKBufferView::~VKBufferView()=default;

    void VKBufferView::Destroy()
    {
        delete this;
    }


    void VKBufferView::CreateVKDescriptor(const BufferViewCreateInfo* createInfo)
    {
        //TODO
    }
}