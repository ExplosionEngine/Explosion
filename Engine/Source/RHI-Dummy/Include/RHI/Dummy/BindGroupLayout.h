//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/BindGroupLayout.h>

namespace RHI::Dummy {
    class DummyBindGroupLayout final : public BindGroupLayout {
    public:
        NonCopyable(DummyBindGroupLayout)
        explicit DummyBindGroupLayout(const BindGroupLayoutCreateInfo& createInfo);
        ~DummyBindGroupLayout() override;
    };
}
