#ifndef EXPLOSION_WINDOW_H
#define EXPLOSION_WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Render/RenderTarget.h>

namespace Explosion {
class Window: public RenderTarget {
private:
    explicit Window() = default;
    ~Window() override;

    GLFWwindow* window = nullptr;
protected:
public:
    static Window* Create();

    bool Init(int width, int height, const char *name) override;
    bool IsValid() override;
    void Flush() override;
};
}

#endif
