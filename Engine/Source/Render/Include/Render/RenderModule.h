//
// Created by johnk on 2023/8/4.
//

#pragma once

#include <Core/Module.h>
#include <RHI/RHI.h>
#include <Render/Api.h>
#include <Render/Renderer.h>
#include <Render/RenderThread.h>
#include <Render/Scene.h>
#include <Render/View.h>

namespace Render {
    struct RenderModuleInitParams {
        RHI::RHIType rhiType;
    };

    class RENDER_API RenderModule final : public Core::Module {
    public:
        RenderModule();
        ~RenderModule() override;

        void OnLoad() override;
        void OnUnload() override;
        Core::ModuleType Type() const override;

        void Initialize(const RenderModuleInitParams& inParams);
        void DeInitialize();
        RHI::Device* GetDevice() const;
        Render::RenderThread& GetRenderThread() const;
        Scene* NewScene() const;
        ViewState* NewViewState() const;
        View CreateView() const;
        StandardRenderer CreateStandardRenderer(const StandardRenderer::Params& inParams) const;

    private:
        bool initialized;
        RHI::Instance* rhiInstance;
        Common::UniquePtr<RHI::Device> rhiDevice;
    };
}
