#ifndef EXPLOSION_RENDERER_H
#define EXPLOSION_RENDERER_H

namespace Explosion {
class Renderer {
protected:
    explicit Renderer() = default;
    virtual ~Renderer() = default;

    virtual void DrawFrame() = 0;

    int width = 0;
    int height = 0;
    const char* name = nullptr;
public:
    static void Destroy(Renderer* renderer);

    virtual bool Init(int wWidth, int wHeight, const char* wName) = 0;
    virtual bool IsValid() = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;

    bool Resize(int wWidth, int wHeight);
};
}

#endif
