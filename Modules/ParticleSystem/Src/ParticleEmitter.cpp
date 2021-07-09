//
// Created by LiZhen on 2021/5/30.
//

#include <ParticleEmitter.h>

#include <Common/Exception.h>

namespace Explosion {

    namespace {
        constexpr float PARTICLE_MIN_DURATION = 0.001;
    }

    void ParticleEmitter::SetDuration(float value)
    {
        duration = value;
        EXPLOSION_ASSERT(duration >= PARTICLE_MIN_DURATION, "duration too short");
    }

    void ParticleEmitter::Tick(float delta)
    {
        time += delta;
        if (time > duration) {
            time = std::fmod(time, duration);
        }

        PreTick(delta);

        Spawn(delta);

        PostTick();
    }

    void ParticleEmitter::PreTick(float delta)
    {
        Recycle();

        Update(delta);
    }

    void ParticleEmitter::Spawn(float delta)
    {
        uint32_t count = std::floor(rate * delta);
        uint32_t lastSize = currentSize;
        currentSize += count;
        if (currentSize > capacity) {
            Resize(currentSize);
        }

        float leftTime = duration - time;
        Particle* base = &particleBase[lastSize];
        for (uint32_t i = 0; i < count; ++i) {
            Particle& particle = base[i];
            ResetParticle(particle);
            for (auto& spawn : spawnEffectors) {
                spawn->Spawn(particle, leftTime);
            }

            if (particle.current > particle.lifetime) {
                --i;
                --count;
                --currentSize;
            }
        }
    }

    void ParticleEmitter::PostTick()
    {
    }

    void ParticleEmitter::Update(float delta)
    {
        for (auto& effector : updateEffectors) {
            effector->Update(particleBase, currentSize, delta);
        }

        for (uint32_t i = 0; i < currentSize; ++i) {
            Particle& particle = particleBase[i];
            particle.position += particle.velocity * delta;
        }
    }

    void ParticleEmitter::Recycle()
    {
        for (uint32_t i = 0; i < currentSize;++i) {
            Particle& particle = particleBase[i];
            if (particle.current > particle.lifetime) {
                particleBase[i] = particleBase[currentSize - 1];
                --i;
                --currentSize;
            }
        }
    }

    void ParticleEmitter::Resize(uint32_t newSize)
    {
        while (newSize > capacity) {
            capacity *= 2;
            capacity = std::max(64u, capacity);
        }
        particleData.resize(capacity);
        indicesData.resize(capacity);
        particleBase = particleData.data();
    }

    void ParticleEmitter::AddEffector(ParticleEffector* effector, EffectorFlags flag)
    {
        if (flag & PARTICLE_EFFECT_SPAWN) {
            spawnEffectors.emplace_back(effector);
        }
        if (flag & PARTICLE_EFFECT_UPDATE) {
            updateEffectors.emplace_back(effector);
        }
    }

}