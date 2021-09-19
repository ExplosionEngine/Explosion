//
// Created by Zach Lee on 2021/5/30.
//

#ifndef EXPLOSION_PARTICLELIFETIMEEFFECTOR_H
#define EXPLOSION_PARTICLELIFETIMEEFFECTOR_H

#include <ParticleEffector.h>
#include <ParticleSystem/Api.h>

namespace Explosion {

    class PARTICLE_API ParticleLifeTimeEffector : public ParticleEffector {
    public:
        ParticleLifeTimeEffector() = default;
        ~ParticleLifeTimeEffector() = default;

    private:
        void Spawn(Particle& particle, float time) override;
        void Update(Particle* particleBase, uint32_t activeNum, float time) override;
        float lifeTime = 2.f;
    };
}

#endif //EXPLOSION_PARTICLELIFETIMEEFFECTOR_H
