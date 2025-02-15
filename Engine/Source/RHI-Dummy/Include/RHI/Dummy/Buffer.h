//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <vector>
#include <RHI/Buffer.h>

namespace RHI::Dummy {
    class DummyBuffer final : public Buffer {
    public:
        NonCopyable(DummyBuffer)
        explicit DummyBuffer(const BufferCreateInfo& createInfo);
        ~DummyBuffer() override;

        void* Map(MapMode mapMode, size_t offset, size_t length) override;
        void UnMap() override;
        Common::UniquePtr<BufferView> CreateBufferView(const BufferViewCreateInfo& createInfo) override;
    private:
        std::vector<uint8_t> dummyData;
    };
}
