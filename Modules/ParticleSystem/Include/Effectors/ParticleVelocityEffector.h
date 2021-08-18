//
// Created by LiZhen on 2021/5/30.
//

#ifndef EXPLOSION_PARTICLEVELOCITYEFFECTOR_H
#define EXPLOSION_PARTICLEVELOCITYEFFECTOR_H

#include <ParticleEffector.h>
#include <ParticleSystem/Api.h>

namespace Explosion {

    class PARTICLE_API ParticleVelocityEffector : public ParticleEffector {
    public:
        ParticleVelocityEffector() = default;
        ~ParticleVelocityEffector() = default;
    };

    class PARTICLE_API ParticleConeEffector : public ParticleVelocityEffector {
    public:
        ParticleConeEffector() = default;
        ~ParticleConeEffector() = default;

    private:
        virtual void Spawn(Particle& particle, float time);
        float angle = 15.f / 180.f * 3.14f;
        float initialVelocity = 5.f;
    };
}

#endif //EXPLOSION_PARTICLEVELOCITYEFFECTOR_H
