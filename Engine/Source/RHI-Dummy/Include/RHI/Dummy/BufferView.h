//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/BufferView.h>

namespace RHI::Dummy {
    class DummyBufferView final : public BufferView {
    public:
        NonCopyable(DummyBufferView)
        explicit DummyBufferView(const BufferViewCreateInfo& createInfo);
        ~DummyBufferView() override;
    };
}
