//
// Created by John Kindem on 2021/6/3.
//

#include <Engine/Engine.h>
#include <Engine/World.h>
#include <Engine/Render/Render.h>
#include <chrono>

namespace Explosion {
    Engine* Engine::GetInstance()
    {
        static auto* instance = new Engine();
        return instance;
    }

    World* Engine::CreateWorld()
    {
        worlds.emplace_back(new World);
        return worlds.back();
    }

    void Engine::DestroyWorld(World* world)
    {
        auto iter = std::find(worlds.begin(), worlds.end(), world);
        if (iter != worlds.end()) {
            delete *iter;
            worlds.erase(iter);
        }
    }

    void Engine::Start(const StartInfo& info)
    {
        Render::Descriptor rci;
        rci.rhiName = info.rhiName;
        render = Render::CreateRender(rci);
    }

    void Engine::Stop()
    {
        Render::DestroyRender(render);
    }

    void Engine::Tick()
    {
        static auto timePoint = std::chrono::steady_clock::now();
        auto current = std::chrono::steady_clock::now();
        auto delta = std::chrono::duration<float, std::milli>(current - timePoint).count();
        timePoint = current;

        for (auto& world : worlds) {
            world->Tick(delta);
        }

        render->Tick(delta);
    }
}
