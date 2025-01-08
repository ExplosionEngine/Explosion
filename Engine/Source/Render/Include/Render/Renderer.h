//
// Created by johnk on 2022/8/3.
//

#pragma once

#include <Render/Scene.h>
#include <Render/View.h>
#include <Render/Surface.h>

namespace Render {
    class Renderer {
    public:
        Renderer(const Scene* inScene, const std::vector<const View*>& inViews, const std::vector<const Surface*>& inSurfaces);
        virtual ~Renderer();

        virtual void Render(float inDeltaTimeSeconds) = 0;

    private:
        const Scene* scene;
        const std::vector<const View*>& views;
        const std::vector<const Surface*>& surfaces;
    };
}
