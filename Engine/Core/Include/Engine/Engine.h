//
// Created by John Kindem on 2021/6/3.
//

#ifndef EXPLOSION_ENGINE_H
#define EXPLOSION_ENGINE_H

#include <string>
#include <vector>

namespace Explosion {
    class World;
    class Render;

    class Engine {
    public:
        static Engine* GetInstance();

        struct StartInfo {
            std::string rhiName;
        };

        void Start(const StartInfo& info);

        void Stop();

        void Tick();

        World* CreateWorld();
        void DestroyWorld(World* world);

    private:
        Render* render = nullptr;
        std::vector<World*> worlds;
    };
}

#endif //EXPLOSION_ENGINE_H
