#include <Render/RenderTarget.h>
using namespace Explosion;

void RenderTarget::Destroy(Explosion::RenderTarget *renderTarget)
{
    delete renderTarget;
}
