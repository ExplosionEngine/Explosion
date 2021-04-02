//
// Created by Administrator on 2021/4/2 0002.
//

#ifndef EXPLOSION_WORLD_H
#define EXPLOSION_WORLD_H

#include <string>

namespace Explosion {
    class World {
    public:
        World(std::string name);
        ~World();

    private:
        std::string name;
    };
}

#endif //EXPLOSION_WORLD_H
