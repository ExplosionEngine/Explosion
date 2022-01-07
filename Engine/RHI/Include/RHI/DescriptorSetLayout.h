//
// Created by johnk on 8/1/2022.
//

#ifndef EXPLOSION_RHI_DESCRIPTOR_SET_LAYOUT_H
#define EXPLOSION_RHI_DESCRIPTOR_SET_LAYOUT_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    struct DescriptorSetLayoutBinding {
        size_t binding;
        DescriptorType type;
        size_t num;
        ShaderStage stage;
    };

    struct DescriptorSetLayoutCreateInfo {
        size_t bindingNum;
        const DescriptorSetLayoutBinding* bindings;
    };

    class DescriptorSetLayout {
    public:
        NON_COPYABLE(DescriptorSetLayout)
        virtual ~DescriptorSetLayout();

    protected:
        explicit DescriptorSetLayout(const DescriptorSetLayoutCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_DESCRIPTOR_SET_LAYOUT_H
