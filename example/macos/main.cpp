#include <iostream>
using namespace std;

#include <Render/Window.h>
#include <Render/Renderer.h>
#include <Render/GLRenderer.h>
using namespace Explosion;

int main()
{
    Window* window = Window::Create();
    if (!window->Init(1024, 768, "Explosion Example")) {
        cout << "failed to init widow" << endl;
        return -1;
    }

    Renderer* renderer = GLRenderer::Create();
    renderer->SetRenderTarget(window);

    while (renderer->IsValid()) {
        renderer->BeginFrame();
        glClearColor(.2f, .3f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        renderer->EndFrame();
    }

    Renderer::Destroy(renderer);
    Window::Destroy(window);
    return 0;
}
