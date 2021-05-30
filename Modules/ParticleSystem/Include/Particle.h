//
// Created by LiZhen on 2021/5/30.
//

#include <Explosion/Common/Math.h>

#ifndef EXPLOSION_PARTICLE_H
#define EXPLOSION_PARTICLE_H

namespace Explosion {

    struct Particle {
        Math::Vector<3> position;
        float current;

        Math::Vector<3> velocity;
        float lifetime;

        Math::Vector<4> size;
        Math::Vector<4> color;
    };

    inline void ResetParticle(Particle& particle)
    {
        particle.position = Math::Vector<3>{0.f, 0.f, 0.f};
        particle.velocity = Math::Vector<3>{0.f, 0.f, 0.f};
        particle.size     = Math::Vector<4>{1.f, 1.f, 1.f, 0.f};
        particle.color    = Math::Vector<4>{1.f, 1.f, 1.f, 1.f};
        particle.current  = 0.f;
        particle.lifetime = 0.f;
    }

}

#endif //EXPLOSION_PARTICLE_H
