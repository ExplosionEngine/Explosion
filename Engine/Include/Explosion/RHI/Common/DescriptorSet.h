//
// Created by John Kindem on 2021/5/26.
//

#ifndef EXPLOSION_DESCRIPTORSET_H
#define EXPLOSION_DESCRIPTORSET_H

#include <Explosion/RHI/Common/Enum.h>

namespace Explosion::RHI {
    class DescriptorPool;
    class GraphicsPipeline;

    class DescriptorSet {
    public:
        struct DescriptorWriteInfo {
            uint32_t binding = 0;
            DescriptorType type = DescriptorType::MAX;
            // TODO buffer info
            // TODO texture info
        };

        virtual ~DescriptorSet();
        virtual void WriteDescriptors(const std::vector<DescriptorWriteInfo>& writeInfos) = 0;

    protected:
        DescriptorSet();
    };
}

#endif //EXPLOSION_DESCRIPTORSET_H
