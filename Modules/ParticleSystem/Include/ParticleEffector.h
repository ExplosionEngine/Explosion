//
// Created by LiZhen on 2021/5/30.
//

#ifndef EXPLOSION_PARTICLEEFFECTOR_H
#define EXPLOSION_PARTICLEEFFECTOR_H

#include <Particle.h>
#include <ParticleSystemApi.h>
#include <Random.h>

namespace Explosion {

    enum EffectorType {
        PARTICLE_EFFECT_NONE   = 0x00,
        PARTICLE_EFFECT_SPAWN  = 0x01,
        PARTICLE_EFFECT_UPDATE = 0x02,
        PARTICLE_EFFECT_BOTH   = PARTICLE_EFFECT_SPAWN | PARTICLE_EFFECT_UPDATE
    };
    using EffectorFlags = uint32_t;

    class PARTICLE_API ParticleEffector {
    public:
        ParticleEffector() = default;
        virtual ~ParticleEffector() = default;

        void SetRandomDevice(RandomDevice& randomDevice);

        virtual void Spawn(Particle& particle, float time) {}
        virtual void Update(Particle* particleBase, uint32_t activeNum, float time) {}

    protected:
        RandomDevice* rd;
    };
}

#endif //EXPLOSION_PARTICLEEFFECTOR_H
