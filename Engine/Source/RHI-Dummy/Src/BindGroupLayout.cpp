//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/BindGroupLayout.h>

namespace RHI::Dummy {
    DummyBindGroupLayout::DummyBindGroupLayout(const BindGroupLayoutCreateInfo* createInfo)
        : BindGroupLayout(createInfo)
    {
    }

    DummyBindGroupLayout::~DummyBindGroupLayout() = default;

    void DummyBindGroupLayout::Destroy()
    {
        delete this;
    }
}
