//
// Created by Zach Lee on 2021/5/30.
//

#include <Effectors/ParticleLifeTimeEffector.h>

namespace Explosion {

    void ParticleLifeTimeEffector::Spawn(Particle& particle, float time)
    {
        particle.lifetime = lifeTime;
        particle.current = 0.f;
    }

    void ParticleLifeTimeEffector::Update(Particle* particleBase, uint32_t activeNum, float time)
    {
        for (uint32_t i = 0; i < activeNum; ++i) {
            particleBase[i].current += time;
        }
    }

}