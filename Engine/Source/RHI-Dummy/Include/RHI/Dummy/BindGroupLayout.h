//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/BindGroupLayout.h>

namespace RHI::Dummy {
    class DummyBindGroupLayout : public BindGroupLayout {
    public:
        explicit DummyBindGroupLayout(const BindGroupLayoutCreateInfo* createInfo);
        ~DummyBindGroupLayout() override;

        void Destroy() override;
    };
}
