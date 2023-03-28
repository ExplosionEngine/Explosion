//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/BufferView.h>

namespace RHI::Dummy {
    DummyBufferView::DummyBufferView(const BufferViewCreateInfo& createInfo)
        : BufferView(createInfo)
    {
    }

    DummyBufferView::~DummyBufferView() = default;

    void DummyBufferView::Destroy()
    {
        delete this;
    }
}
