//
// Created by johnk on 2022/8/3.
//

#include <Render/Renderer.h>

namespace Render {
    Renderer::Renderer(const Params& inParams)
        : device(inParams.device)
        , scene(inParams.scene)
        , surface(inParams.surface)
        , surfaceExtent(inParams.surfaceExtent)
        , views(inParams.views)
        , waitSemaphore(inParams.waitSemaphore)
        , signalSemaphore(inParams.signalSemaphore)
        , signalFence(inParams.signalFence)
    {
    }

    Renderer::~Renderer() = default;

    StandardRenderer::StandardRenderer(const Params& inParams)
        : Renderer(inParams)
        , rgBuilder(*device)
    {
    }

    StandardRenderer::~StandardRenderer() = default;

    void StandardRenderer::Render(float inDeltaTimeSeconds)
    {
        // TODO
        FinalizeViews();
    }

    void StandardRenderer::FinalizeViews() const
    {
        for (const auto& view : views) {
            view.state->prevData = view.data;
        }
    }
}
