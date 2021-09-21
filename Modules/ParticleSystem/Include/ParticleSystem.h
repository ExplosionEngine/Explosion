//
// Created by Zach Lee on 2021/5/30.
//

#ifndef EXPLOSION_PARTICLESYSTEM_H
#define EXPLOSION_PARTICLESYSTEM_H

#include <vector>
#include <Particle.h>
#include <ParticleEmitter.h>
#include <ParticleSystem/Api.h>

namespace Explosion {

    class PARTICLE_API ParticleSystem {
    public:
        ParticleSystem() {}
        ~ParticleSystem();

        template <typename T>
        T* CreateEmitter(const typename T::Descriptor& desc)
        {
            static_assert(std::is_base_of_v<ParticleEmitter, T>, "T must be a ParticleEmitter");
            T* res = new T(desc);
            emitters.emplace_back(res);
            return res;
        }

        void Tick(float delta);

    private:
        std::vector<ParticleEmitter*> emitters;
    };

}
#endif //EXPLOSION_PARTICLESYSTEM_H
