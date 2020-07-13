#include <Render/OpenGLRenderer.h>
using namespace Explosion;

Renderer * OpenGLRenderer::Create()
{
    return new OpenGLRenderer();
}

OpenGLRenderer::~OpenGLRenderer()
{
    if (window != nullptr) {
        glfwDestroyWindow(window);
    }
}

bool OpenGLRenderer::Init(int wWidth, int wHeight, const char* wName)
{
    if (window != nullptr) {
        glfwDestroyWindow(window);
    }
    if (wWidth < 0 || wHeight < 0) {
        return false;
    }

    width = wWidth;
    height = wHeight;
    name = wName;

    if (!glfwInit()) {
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    window = glfwCreateWindow(wWidth, wHeight, name, nullptr, nullptr);
    if (window == nullptr) {
        return false;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        return false;
    }
    glViewport(0, 0, width, height);

    return true;
}

bool OpenGLRenderer::IsValid()
{
    if (window == nullptr) {
        return false;
    }
    return !glfwWindowShouldClose(window);
}

void OpenGLRenderer::BeginFrame()
{

}

void OpenGLRenderer::EndFrame()
{
    DrawFrame();
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void OpenGLRenderer::DrawFrame()
{
    glClearColor(.2f, .3f, .3f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
}