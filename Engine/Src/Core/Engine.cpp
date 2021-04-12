//
// Created by John Kindem on 2021/3/23.
//

#include <Explosion/Core/Engine.h>
#include <Explosion/Core/World.h>
#include <Explosion/Core/Renderer.h>

namespace Explosion {
    Engine* Engine::GetInstance()
    {
        static auto* instance = new Engine();
        return instance;
    }

    World* Engine::CreateWorld(const std::string& name)
    {
        return new World(name);
    }

    void Engine::DestroyWorld(World* world)
    {
        delete world;
    }

    Renderer* Engine::CreateRenderer(void* surface, uint32_t width, uint32_t height)
    {
        return new Renderer(driver, surface, width, height);
    }

    void Engine::DestroyRenderer(Renderer* renderer)
    {
        delete renderer;
    }
}
