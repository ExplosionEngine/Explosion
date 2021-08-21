//
// Created by LiZhen on 2021/5/30.
//

#ifndef EXPLOSION_PARTICLEEMITTER_H
#define EXPLOSION_PARTICLEEMITTER_H

#include <cstdint>
#include <vector>
#include <ParticleEffector.h>
#include <ParticleSystem/Api.h>

namespace Explosion {
    class PARTICLE_API ParticleEmitter {
    public:

        ParticleEmitter()
            : capacity(0)
            , currentSize(0)
            , time(0.f)
            , duration(5.f)
            , rate(100.f)
            , particleBase(nullptr)
        {
        }

        virtual ~ParticleEmitter() = default;

        void SetDuration(float duration);

        void Tick(float delta);

        void AddEffector(ParticleEffector* effector, EffectorFlags flag);

        uint32_t GetPoolSize() const { return capacity * sizeof(Particle); }

        uint32_t GetActiveCount() const { return currentSize; }

        const uint8_t* GetVertexData() const { return (uint8_t*)particleData.data(); }

    protected:
        virtual void PreTick(float delta);
        virtual void Spawn(float delta);
        virtual void PostTick();

        virtual void Resize(uint32_t size);

        virtual void Update(float delta);

        void Recycle();

    private:
        uint32_t capacity;
        uint32_t currentSize;

        float time;
        float duration;
        float rate;

        Particle* particleBase;
        std::vector<Particle> particleData;
        std::vector<uint32_t> indicesData;

        std::vector<ParticleEffector*> spawnEffectors;
        std::vector<ParticleEffector*> updateEffectors;
    };

}
#endif //EXPLOSION_PARTICLEEMITTER_H
