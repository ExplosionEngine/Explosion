//
// Created by John Kindem on 2021/5/26.
//

#ifndef EXPLOSION_DESCRIPTORPOOL_H
#define EXPLOSION_DESCRIPTORPOOL_H

#include <vector>

#include <RHI/Enum.h>

namespace Explosion::RHI {
    class DescriptorPool {
    public:
        struct PoolSize {
            DescriptorType type;
            uint32_t count;
        };

        struct Config {
            std::vector<PoolSize> poolSizes;
            uint32_t maxSets = 0;
        };

        virtual ~DescriptorPool();

    protected:
        explicit DescriptorPool(Config config);

        Config config;
    };
}

#endif //EXPLOSION_DESCRIPTORPOOL_H
