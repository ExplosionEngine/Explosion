//
// Created by johnk on 2022/8/3.
//

#pragma once

#include <cstdint>
#include <string>

#include <Render/Canvas.h>
#include <Render/SceneView.h>

namespace Rendering {
    struct RendererInitializer {
        std::string rhiString;
    };

    class Renderer {
    public:
        static Renderer& Get();
        ~Renderer();

        void Initialize(const RendererInitializer& inInitializer);
        Render::Canvas* CreateCanvas(void* nativeWindow, uint32_t width, uint32_t height);
        void RenderFrame(Render::Canvas* inCanvas, const Render::SceneView& inSceneView);

    private:
        Renderer();
    };
}
