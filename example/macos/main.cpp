#include <Render/OpenGLRenderer.h>
#include <Render/Renderer.h>
using namespace Explosion;

#include <iostream>
using namespace std;

int main()
{
    Renderer* renderer = OpenGLRenderer::Create();
    if (!renderer->Init(800, 600, "macos-example")) {
        cout << "failed to init renderer" << endl;
        Renderer::Destroy(renderer);
        return -1;
    }

    while (renderer->IsValid()) {
        renderer->BeginFrame();
        renderer->EndFrame();
    }

    Renderer::Destroy(renderer);
    return 0;
}
