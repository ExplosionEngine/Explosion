#include <Engine/Engine.h>
#include <Engine/ECS.h>
#include <Engine/World.h>
#include <thread>

using namespace Explosion;

int main()
{
    Engine* engine = Engine::GetInstance();

    Engine::StartInfo info = {"RHIVulkan"};
    engine->Start(info);

    World* world = engine->CreateWorld();

    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        engine->Tick();
    }

    engine->DestroyWorld(world);

    engine->Stop();
    return 0;
}