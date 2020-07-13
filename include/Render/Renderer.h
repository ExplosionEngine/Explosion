#ifndef EXPLOSION_RENDERER_H
#define EXPLOSION_RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Explosion {
class Renderer {
private:
    explicit Renderer() = default;
    ~Renderer();
    void DrawFrame();

    GLFWwindow* window = nullptr;
    int width = 0;
    int height = 0;
    const char* name = nullptr;
public:
    static Renderer* Create();
    static void Destroy(Renderer* renderer);

    bool Init(int wWidth, int wHeight, const char* wName);
    bool Resize(int wWidth, int wHeight);
    bool IsValid();
    void BeginFrame();
    void EndFrame();
};
}

#endif
