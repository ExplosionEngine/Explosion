#include <Render/Renderer.h>
using namespace Explosion;

void Renderer::Destroy(Renderer *renderer)
{
    delete renderer;
}

void Renderer::SetRenderTarget(RenderTarget *renderTarget)
{
    this->renderTarget = renderTarget;
}