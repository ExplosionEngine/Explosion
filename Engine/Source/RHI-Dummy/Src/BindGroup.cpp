//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/BindGroup.h>

namespace RHI::Dummy {
    DummyBindGroup::DummyBindGroup(const BindGroupCreateInfo& createInfo)
        : BindGroup(createInfo)
    {
    }

    DummyBindGroup::~DummyBindGroup() = default;
}
