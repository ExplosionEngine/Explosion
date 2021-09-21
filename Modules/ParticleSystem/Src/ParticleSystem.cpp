//
// Created by Zach Lee on 2021/5/30.
//

#include <ParticleSystem.h>

namespace Explosion {

    ParticleSystem::~ParticleSystem()
    {
        for (auto emitter : emitters) {
            delete emitter;
        }
    }

    void ParticleSystem::Tick(float delta)
    {
        for (auto emitter : emitters) {
            emitter->Tick(delta);
        }
    }
}
