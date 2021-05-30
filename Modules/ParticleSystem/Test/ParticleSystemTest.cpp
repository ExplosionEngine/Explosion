//
// Created by LiZhen on 2021/5/30.
//

#include <gtest/gtest.h>
#include <ParticleSystem.h>
#include <Emitters/ParticleSpriteEmitter.h>
#include <Effectors/ParticleLocationEffector.h>
#include <Effectors/ParticleLifeTimeEffector.h>

using namespace Explosion;

class ParticleSystemTest : public testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ParticleSystemTest, TestParticleSystem01) {
    ParticleSystem system;

    auto emitter = system.CreateEmitter<ParticleSpriteEmitter>(ParticleSpriteEmitter::Descriptor{});
    RandomDevice random;

    ParticleBoxLocation location;
    location.SetRandomDevice(random);

    ParticleLifeTimeEffector lifetime;
    emitter->AddEffector(&lifetime, PARTICLE_EFFECT_BOTH);
    emitter->AddEffector(&location, PARTICLE_EFFECT_SPAWN);

    system.Tick(0.1f);
    system.Tick(0.1f);

}