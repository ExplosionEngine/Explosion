//
// Created by johnk on 2022/8/3.
//

#pragma once

#include <cstdint>

#include <Render/Canvas.h>
#include <Render/SceneView.h>

namespace Renderer {
    class Renderer {
    public:
        static Renderer& Get();
        ~Renderer();

        Render::Canvas* CreateCanvas(void* nativeWindow, uint32_t width, uint32_t height);
        void RenderFrame(Render::Canvas* inCanvas, const Render::SceneView& inSceneView);

    private:
        Renderer();
    };
}
