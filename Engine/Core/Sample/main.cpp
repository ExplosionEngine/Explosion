#include <Engine/Render/Render.h>

using namespace Explosion;

int main()
{
    RenderCreateInfo ci;
    ci.rhiName = "RHIVulkan";

    auto render = Render::CreateRender(ci);

    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        render->Tick(0.f);
    }

    Render::DestroyRender();
    return 0;
}