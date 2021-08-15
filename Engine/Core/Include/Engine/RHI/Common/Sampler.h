//
// Created by Administrator on 2021/5/29 0029.
//

#ifndef EXPLOSION_SAMPLER_H
#define EXPLOSION_SAMPLER_H

#include <Engine/RHI/Common/Enum.h>

namespace Explosion::RHI {
    class Sampler {
    public:
        struct Config {
            SamplerFilter magFilter;
            SamplerFilter minFilter;
            SamplerAddressMode addressModeU;
            SamplerAddressMode addressModeV;
            SamplerAddressMode addressModeW;
            bool anisotropyEnabled;
            float maxAnisotropy;
            BorderColor borderColor;
            bool unNormalizedCoordinates;
            bool compareEnabled;
            CompareOp compareOp;
            SamplerMipmapMode mipmapMode;
            float mipLodBias;
            float minLod;
            float maxLod;
        };

        virtual ~Sampler();

    protected:
        explicit Sampler(Config config);

        Config config;
    };
}

#endif //EXPLOSION_SAMPLER_H
