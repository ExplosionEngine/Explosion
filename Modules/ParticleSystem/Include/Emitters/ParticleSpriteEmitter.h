//
// Created by LiZhen on 2021/5/30.
//

#ifndef EXPLOSION_PARTICLESPRITEEMITTER_H
#define EXPLOSION_PARTICLESPRITEEMITTER_H

#include <ParticleEmitter.h>
#include <ParticleSystemApi.h>

namespace Explosion {

    class PARTICLE_API ParticleSpriteEmitter : public ParticleEmitter {
    public:
        struct Descriptor {};

        ParticleSpriteEmitter(const Descriptor& desc) : descriptor(desc) {}
        ~ParticleSpriteEmitter() {}

    private:
        Descriptor descriptor;
    };

}

#endif //EXPLOSION_PARTICLESPRITEEMITTER_H
