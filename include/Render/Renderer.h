#ifndef EXPLOSION_RENDERER_H
#define EXPLOSION_RENDERER_H

#include <Render/RenderTarget.h>

namespace Explosion {
class Renderer {
private:
protected:
    Renderer() = default;
    virtual ~Renderer() = default;
protected:
    virtual void DrawFrame() = 0;

    RenderTarget* renderTarget = nullptr;
public:
    static void Destroy(Renderer* renderer);

    void SetRenderTarget(RenderTarget* renderTarget);

    virtual bool IsValid() = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
};
}

#endif
