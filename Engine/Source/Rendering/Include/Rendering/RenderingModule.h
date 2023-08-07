//
// Created by johnk on 2023/8/4.
//

#pragma once

#include <Core/Module.h>
#include <Render/Scene.h>
#include <Rendering/Api.h>

namespace Rendering {
    class RENDERING_API RenderingModule : public Core::Module {
    public:
        RenderingModule();
        ~RenderingModule() override;

        Render::SceneInterface* AllocateScene();
        void DestroyScene(Render::SceneInterface* inScene);
    };
}

IMPLEMENT_MODULE(RENDERING_API, Rendering::RenderingModule);
