//
// Created by Zach Lee on 2021/5/30.
//

#include <Effectors/ParticleForceEffector.h>

namespace Explosion {

    void ParticleGravityEffector::Update(Particle* particleBase, uint32_t activeNum, float time)
    {
        for (uint32_t i = 0; i < activeNum; ++i) {
            Particle& particle = particleBase[i];
            particle.velocity.y += gravity * time;
        }
    }

}
