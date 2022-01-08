//
// Created by johnk on 8/1/2022.
//

#ifndef EXPLOSION_RHI_DESCRIPTOR_SET_H
#define EXPLOSION_RHI_DESCRIPTOR_SET_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    class DescriptorPool;
    class DescriptorSetLayout;
    class DescriptorSet;
    class Buffer;

    struct DescriptorSetAllocateInfo {
        const DescriptorPool* descriptorPool;
        size_t descriptorSetNum;
        const DescriptorSetLayout* layouts;
    };

    struct DescriptorBufferInfo {
        Buffer* buffer;
        size_t offset;
        size_t range;
    };

    struct DescriptorImageInfo {
        // TODO
    };

    struct DescriptorTexelBufferView {
        // TODO
    };

    struct DescriptorWriteInfo {
        const DescriptorSet* descriptorSet;
        size_t binding;
        size_t arrayElement;
        DescriptorType descriptorType;
        size_t descriptorNum;
        const DescriptorBufferInfo* bufferInfo;
        const DescriptorImageInfo* imageInfo;
        const DescriptorTexelBufferView* texelBufferView;
    };

    class DescriptorSet {
    public:
        NON_COPYABLE(DescriptorSet)
        virtual ~DescriptorSet();

    protected:
        explicit DescriptorSet(const DescriptorSetAllocateInfo* allocateInfo);
    };
}

#endif //EXPLOSION_RHI_DESCRIPTOR_SET_H
