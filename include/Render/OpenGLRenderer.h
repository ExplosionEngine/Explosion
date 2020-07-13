#ifndef EXPLOSION_OPENGL_RENDERER_H
#define EXPLOSION_OPENGL_RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Render/Renderer.h>

namespace Explosion {
class OpenGLRenderer: public Renderer {
private:
    OpenGLRenderer(): Renderer() {}
    ~OpenGLRenderer() override;

    GLFWwindow* window = nullptr;
protected:
    void DrawFrame() override;
public:
    static Renderer* Create();

    bool Init(int wWidth, int wHeight, const char* wName) override;
    bool IsValid() override;
    void BeginFrame() override;
    void EndFrame() override;
};
}

#endif
