//
// Created by johnk on 8/1/2022.
//

#ifndef EXPLOSION_RHI_DESCRIPTOR_POOL_H
#define EXPLOSION_RHI_DESCRIPTOR_POOL_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    struct DescriptorPoolSize {
        DescriptorType type;
        size_t count;
    };

    struct DescriptorPoolCreateInfo {
        size_t poolSizeNum;
        const DescriptorPoolSize* poolSizes;
    };

    class DescriptorPool {
    public:
        NON_COPYABLE(DescriptorPool)
        virtual ~DescriptorPool();

    protected:
        explicit DescriptorPool(const DescriptorPoolCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_DESCRIPTOR_POOL_H
