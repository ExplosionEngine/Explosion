//
// Created by LiZhen on 2021/5/30.
//

#ifndef EXPLOSION_PARTICLELOCATIONEFFECTOR_H
#define EXPLOSION_PARTICLELOCATIONEFFECTOR_H

#include <Math/Math.h>

#include <ParticleEffector.h>
#include <ParticleSystem/Api.h>

namespace Explosion {

    class PARTICLE_API ParticleLocationEffector : public ParticleEffector {
    public:
        ParticleLocationEffector() = default;
        ~ParticleLocationEffector() = default;
    };

    class PARTICLE_API ParticlePointLocation : public ParticleLocationEffector {
    public:
        ParticlePointLocation() = default;
        ~ParticlePointLocation() = default;

    private:
        void Spawn(Particle& particle, float time) override;
        Math::Vector<3> pivot = {0.f, 0.f, 0.f};
    };

    class PARTICLE_API ParticleBoxLocation : public ParticleLocationEffector {
    public:
        ParticleBoxLocation() = default;
        ~ParticleBoxLocation() = default;

        void SetBox(const Math::Vector<3>& min, const Math::Vector<3>& max);

    protected:
        void Spawn(Particle& particle, float time) override;
        Math::Vector<3> min = {0, 0, 0};
        Math::Vector<3> max = {1, 1, 1};
    };

}
#endif //EXPLOSION_PARTICLELOCATIONEFFECTOR_H
