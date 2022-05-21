//
// Created by johnk on 19/2/2022.
//

#pragma once

#include <Common/Utility.h>
#include <RHI/Common.h>

namespace RHI {
    struct BindGroupLayoutEntry {
        uint8_t binding;
        BindingType type;
        ShaderStageFlags shaderVisibility;
    };

    struct BindGroupLayoutCreateInfo {
        uint8_t layoutIndex;
        uint32_t entryNum;
        const BindGroupLayoutEntry* entries;
    };

    class BindGroupLayout {
    public:
        NON_COPYABLE(BindGroupLayout)
        virtual ~BindGroupLayout();

        virtual void Destroy() = 0;

    protected:
        explicit BindGroupLayout(const BindGroupLayoutCreateInfo* createInfo);
    };
}
