//
// Created by johnk on 2023/8/4.
//

#include <Rendering/RenderingModule.h>
#include <Rendering/Scene.h>

namespace Rendering {
    RenderingModule::RenderingModule() = default;

    RenderingModule::~RenderingModule() = default;

    Render::SceneInterface* RenderingModule::AllocateScene() // NOLINT
    {
        return new Scene();
    }

    void RenderingModule::DestroyScene(Render::SceneInterface* inScene) // NOLINT
    {
        delete inScene;
    }
}
