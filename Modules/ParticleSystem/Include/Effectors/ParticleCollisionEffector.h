//
// Created by LiZhen on 2021/5/30.
//

#ifndef EXPLOSION_PARTICLECOLLISIONEFFECTOR_H
#define EXPLOSION_PARTICLECOLLISIONEFFECTOR_H

#include <ParticleEffector.h>
#include <ParticleSystemApi.h>

namespace Explosion {

    class ParticleCollisionEffector : public ParticleEffector {
    public:
        ParticleCollisionEffector() {}
        ~ParticleCollisionEffector() {}
    };

    class ParticleGroundCollision : public ParticleCollisionEffector {
    public:
        ParticleGroundCollision() {}
        ~ParticleGroundCollision() {}

        void SetGroundHeight(float height);

    private:
        virtual void Update(Particle* particleBase, uint32_t activeNum, float time);
        float groundHeight = 0.f;
    };

}
#endif //EXPLOSION_PARTICLECOLLISIONEFFECTOR_H
