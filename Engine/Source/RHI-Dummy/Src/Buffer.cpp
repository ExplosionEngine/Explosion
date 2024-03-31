//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/Buffer.h>
#include <RHI/Dummy/BufferView.h>

namespace RHI::Dummy {
    DummyBuffer::DummyBuffer(const BufferCreateInfo& createInfo)
        : Buffer(createInfo)
        , dummyData(1)
    {
    }

    DummyBuffer::~DummyBuffer() = default;

    void* DummyBuffer::Map(MapMode mapMode, size_t offset, size_t length)
    {
        return dummyData.data();
    }

    void DummyBuffer::UnMap()
    {
    }

    Holder<BufferView> DummyBuffer::CreateBufferView(const BufferViewCreateInfo& createInfo)
    {
        return Common::UniqueRef<BufferView>(new DummyBufferView(createInfo));
    }

    void DummyBuffer::Destroy()
    {
        delete this;
    }
}
