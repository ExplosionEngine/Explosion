//
// Created by johnk on 2022/8/3.
//

#include <Render/Renderer.h>

namespace Render {
    Renderer::Renderer(const Params& inParams)
        : scene(inParams.scene)
        , surface(inParams.surface)
        , views(inParams.views)
        , waitSemaphore(inParams.waitSemaphore)
        , signalSemaphore(inParams.signalSemaphore)
        , signalFence(inParams.signalFence)
    {
    }

    Renderer::~Renderer() = default;

    StandardRenderer::StandardRenderer(const Params& inParams)
        : Renderer(inParams)
    {
    }

    StandardRenderer::~StandardRenderer() = default;

    void StandardRenderer::Render(float inDeltaTimeSeconds)
    {
        // TODO
    }
}
