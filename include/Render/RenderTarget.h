#ifndef EXPLOSION_RENDER_TARGET_H
#define EXPLOSION_RENDER_TARGET_H

namespace Explosion {
class RenderTarget {
private:
protected:
    RenderTarget() = default;
    virtual ~RenderTarget() = default;

    int width = 0;
    int height = 0;
    const char* name = nullptr;
public:
    static void Destroy(RenderTarget* renderTarget);

    virtual bool Init(int width, int height, const char* name) = 0;
    virtual bool IsValid() = 0;
    virtual void Flush() = 0;
};
}

#endif
