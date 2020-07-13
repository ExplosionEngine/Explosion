#include <iostream>
#include <Render/Renderer.h>
using namespace std;
using namespace Explosion;

int main()
{
    Renderer* renderer = Renderer::Create(RendererType::OpenGL);
    if (!renderer->Init(800, 600, "macos-example")) {
        cout << "failed to init renderer" << endl;
        Renderer::Destroy(renderer);
        return -1;
    }

    while (renderer->IsValid()) {
        renderer->BeginFrame();
        renderer->EndFrame();
    }

    return 0;
}
