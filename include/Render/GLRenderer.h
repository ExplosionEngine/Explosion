#ifndef EXPLOSION_GL_RENDERER_H
#define EXPLOSION_GL_RENDERER_H

#include <Render/Renderer.h>

namespace Explosion {
class GLRenderer: public Renderer {
private:
    GLRenderer() = default;
    ~GLRenderer() override = default;
    void DrawFrame() override;
protected:
public:
    static GLRenderer* Create();

    bool IsValid() override;
    void BeginFrame() override;
    void EndFrame() override;
};
}

#endif
