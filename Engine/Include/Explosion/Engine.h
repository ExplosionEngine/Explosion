//
// Created by John Kindem on 2021/6/3.
//

#ifndef EXPLOSION_ENGINE_H
#define EXPLOSION_ENGINE_H

namespace Explosion {
    class World;
    class Renderer;

    class Engine {
    public:
        static Engine* GetInstance();

        World* CreateWorld();
        void DestroyWorld(World* world);
    };
}

#endif //EXPLOSION_ENGINE_H
