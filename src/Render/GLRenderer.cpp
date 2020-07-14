#include <Render/GLRenderer.h>
#include <glad/glad.h>
using namespace Explosion;

GLRenderer * GLRenderer::Create()
{
    return new GLRenderer();
}

void GLRenderer::DrawFrame()
{

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