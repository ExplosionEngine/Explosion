//
// Created by Zach Lee on 2021/5/30.
//

#include <Effectors/ParticleCollisionEffector.h>

namespace Explosion {

    void ParticleGroundCollision::Update(Particle* particleBase, uint32_t activeNum, float time)
    {
        for (uint32_t i = 0; i < activeNum; ++i) {
            Particle& particle = particleBase[i];
            if (particle.position.y <= groundHeight) {
                particle.velocity.y = std::fabs(particle.velocity.y * 0.6f);
            }
        }
    }

    void ParticleGroundCollision::SetGroundHeight(float height)
    {
        groundHeight = height;
    }

}