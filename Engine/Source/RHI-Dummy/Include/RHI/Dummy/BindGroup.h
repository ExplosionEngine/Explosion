//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/BindGroup.h>

namespace RHI::Dummy {
    class DummyBindGroup final : public BindGroup {
    public:
        NonCopyable(DummyBindGroup)
        explicit DummyBindGroup(const BindGroupCreateInfo& createInfo);
        ~DummyBindGroup() override;
    };
}
