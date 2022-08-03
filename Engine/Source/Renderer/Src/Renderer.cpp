//
// Created by johnk on 2022/8/3.
//

#include <Renderer/Renderer.h>

namespace Renderer {
    Renderer::Renderer() = default;

    Renderer::~Renderer() = default;

    Renderer& Renderer::Get()
    {
        static Renderer instance;
        return instance;
    }

    Render::Canvas* Renderer::CreateCanvas()
    {
        // TODO
        return nullptr;
    }

    void Renderer::RenderFrame(Render::Canvas* inCanvas, const Render::SceneView& inSceneView)
    {
        // TODO
    }
}
