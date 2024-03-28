//
// Created by johnk on 20/3/2022.
//

#include <RHI/BufferView.h>

namespace RHI {
    BufferViewCreateInfo::BufferViewCreateInfo()
        : BufferViewCreateInfoBase<BufferViewCreateInfo>()
    {
    }

    BufferView::BufferView(const BufferViewCreateInfo& createInfo) {}

    BufferView::~BufferView() = default;
}
