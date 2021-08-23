#include <Engine/Render/Render.h>

using namespace Explosion;

int main()
{
    RenderCreateInfo ci;
    ci.rhiName = "RHIVulkan";

    Render render(ci);

    return 0;
}