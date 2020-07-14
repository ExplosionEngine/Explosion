#include <Render/GLRenderer.h>
#include <glad/glad.h>
using namespace Explosion;

GLRenderer * GLRenderer::Create()
{
    return new GLRenderer();
}

void GLRenderer::DrawFrame()
{
    glClearColor(.2f, .3f, .3f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
}

bool GLRenderer::IsValid()
{
    return renderTarget != nullptr && renderTarget->IsValid();
}

void GLRenderer::BeginFrame()
{

}

void GLRenderer::EndFrame()
{
    DrawFrame();
    if (renderTarget != nullptr) {
        renderTarget->Flush();
    }
}