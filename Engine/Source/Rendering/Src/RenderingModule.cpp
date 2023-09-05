//
// Created by johnk on 2023/8/4.
//

#include <Rendering/RenderingModule.h>
#include <Rendering/Scene.h>

namespace Rendering {
    RenderingModule::RenderingModule() = default;

    RenderingModule::~RenderingModule() = default;

    Render::IScene* RenderingModule::AllocateScene() // NOLINT
    {
        return new Scene();
    }

    void RenderingModule::DestroyScene(Render::IScene* inScene) // NOLINT
    {
        delete inScene;
    }

    void RenderingModule::StartupRenderingThread()
    {
        renderingThread = Common::MakeUnique<Common::WorkerThread>("RenderingThread");
    }

    void RenderingModule::ShutdownRenderingThread()
    {
        renderingThread = nullptr;
    }

    void RenderingModule::FlushAllRenderingCommands()
    {
        Assert(renderingThread != nullptr);
        renderingThread->Flush();
    }
}

IMPLEMENT_MODULE(RENDERING_API, Rendering::RenderingModule);
