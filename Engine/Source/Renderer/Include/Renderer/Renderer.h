//
// Created by johnk on 2022/8/3.
//

#pragma once

#include <Render/Canvas.h>
#include <Render/SceneView.h>

namespace Renderer {
    class Renderer {
    public:
        static Renderer& Get();
        ~Renderer();

        Render::Canvas* CreateCanvas();
        void RenderFrame(Render::Canvas* inCanvas, const Render::SceneView& inSceneView);

    private:
        Renderer();
    };
}
