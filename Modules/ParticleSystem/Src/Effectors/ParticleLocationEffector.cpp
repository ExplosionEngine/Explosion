//
// Created by Zach Lee on 2021/5/30.
//

#include <Random.h>
#include <Effectors/ParticleLocationEffector.h>

namespace Explosion {

    void ParticleBoxLocation::SetBox(const Math::Vector<3>& minValue, const Math::Vector<3>& maxValue)
    {
        min = minValue;
        max = maxValue;
    }

    void ParticleBoxLocation::Spawn(Particle& particle, float time)
    {
        std::normal_distribution<float> normalDistX((max.x - min.x) / 2.f, 1.f);
        std::normal_distribution<float> normalDistY((max.y - min.y) / 2.f, 1.f);
        std::normal_distribution<float> normalDistZ((max.z - min.z) / 2.f, 1.f);
        if (rd != nullptr) {
            particle.position.x = rd->Gen(normalDistX);
            particle.position.y = rd->Gen(normalDistY);
            particle.position.z = rd->Gen(normalDistZ);
        }
    }

    void ParticlePointLocation::Spawn(Particle& particle, float time)
    {
        particle.position = pivot;
    }
}