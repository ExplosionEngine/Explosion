//
// Created by LiZhen on 2021/5/30.
//

#include <Effectors/ParticleVelocityEffector.h>

namespace Explosion {

    void ParticleConeEffector::Spawn(Particle& particle, float time)
    {
        std::uniform_real_distribution<float> uDist(0, 360.f / 180.f * 3.14f);
        if (rd != nullptr) {
            float dir = rd->Gen(uDist);
            particle.velocity.x = initialVelocity * sin(angle) * cos(dir);
            particle.velocity.z = initialVelocity * sin(angle) * sin(dir);
            particle.velocity.y = initialVelocity * cos(angle);
        }
    }

}