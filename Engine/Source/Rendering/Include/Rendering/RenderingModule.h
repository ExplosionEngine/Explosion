//
// Created by johnk on 2023/8/4.
//

#pragma once

#include <Core/Module.h>
#include <Rendering/Api.h>

namespace Rendering {
    class RENDERING_API RenderingModule : public Core::Module {
    public:
        RenderingModule();
        ~RenderingModule() override;

        void OnLoad() override;
        void OnUnload() override;
    };
}

IMPLEMENT_MODULE(Rendering::RenderingModule);
