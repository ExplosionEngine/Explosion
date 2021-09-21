//
// Created by Zach Lee on 2021/5/30.
//

#ifndef EXPLOSION_RANDOM_H
#define EXPLOSION_RANDOM_H

#include <random>

namespace Explosion {

    class RandomDevice {
    public:
        RandomDevice() : engine(std::random_device{}()) {}
        virtual ~RandomDevice() = default;

        template <typename Distribution>
        float Gen(Distribution& dist)
        {
            return dist(engine);
        }

    private:
        std::mt19937 engine;
    };

}

#endif //EXPLOSION_RANDOM_H
