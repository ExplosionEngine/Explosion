//
// Created by Administrator on 2021/5/29 0029.
//

#ifndef EXPLOSION_VULKANSAMPLER_H
#define EXPLOSION_VULKANSAMPLER_H

#include <Explosion/RHI/Common/Sampler.h>

namespace Explosion::RHI {
    class VulkanSampler : public Sampler {
    public:
        VulkanSampler();
        ~VulkanSampler();
    };
}

#endif //EXPLOSION_VULKANSAMPLER_H
