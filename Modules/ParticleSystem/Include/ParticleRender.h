//
// Created by liZhen on 2021/5/30.
//

#ifndef EXPLOSION_PARTICLERENDER_H
#define EXPLOSION_PARTICLERENDER_H

#include <ParticleEmitter.h>
#include <ParticleSystem/Api.h>

namespace Explosion {

    class PARTICLE_API ParticleRender {
    public:
        ParticleRender(ParticleEmitter* emi) : emitter(emi) {}
        ~ParticleRender() = default;

    private:
        ParticleEmitter* emitter;
    };

}
#endif //EXPLOSION_PARTICLERENDER_H
