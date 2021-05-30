//
// Created by LiZhen on 2021/5/30.
//

#ifndef EXPLOSION_PARTICLEFORCEEFFECTOR_H
#define EXPLOSION_PARTICLEFORCEEFFECTOR_H

#include <ParticleEffector.h>
#include <ParticleSystemApi.h>

namespace Explosion {

    class PARTICLE_API ParticleForceEffector : public ParticleEffector {
    public:
        ParticleForceEffector() = default;
        ~ParticleForceEffector() = default;
    };

    class PARTICLE_API ParticleGravityEffector : public ParticleForceEffector {
    public:
        ParticleGravityEffector() = default;
        ~ParticleGravityEffector() = default;

        void Update(Particle* particleBase, uint32_t activeNum, float time) override;

    private:
        float gravity = -9.8f;
    };
}

#endif //EXPLOSION_PARTICLEFORCEEFFECTOR_H
