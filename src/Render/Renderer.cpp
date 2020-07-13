#include <Render/Renderer.h>
using namespace Explosion;

void Renderer::Destroy(Renderer *renderer)
{
    delete renderer;
}

bool Renderer::Resize(int wWidth, int wHeight)
{
    return Init(wWidth, wHeight, name);
}
