//
// Created by johnk on 2023/8/4.
//

#include <Rendering/RenderingModule.h>
#include <Rendering/Scene.h>

namespace Rendering {
    RenderingModule::RenderingModule()
        : initialized(false)
        , rhiInstance(nullptr)
    {
    }

    RenderingModule::~RenderingModule() = default;

    void RenderingModule::OnLoad()
    {
        Module::OnLoad();
    }

    void RenderingModule::OnUnload()
    {
        Module::OnUnload();
    }

    void RenderingModule::Initialize(const RenderingModuleInitParams& inParams)
    {
        Assert(!initialized);

        renderingThread = Common::MakeUnique<Common::WorkerThread>("RenderingThread");

        rhiInstance = RHI::Instance::GetByType(inParams.rhiType);
        rhiDevice = rhiInstance->GetGpu(0)->RequestDevice(
            RHI::DeviceCreateInfo()
                .AddQueueRequest(RHI::QueueRequestInfo(RHI::QueueType::graphics, 1))
                .AddQueueRequest(RHI::QueueRequestInfo(RHI::QueueType::compute, 1))
                .AddQueueRequest(RHI::QueueRequestInfo(RHI::QueueType::transfer, 1)));

        initialized = true;
    }

    RHI::Device* RenderingModule::GetDevice() const
    {
        return rhiDevice.Get();
    }

    Render::IScene* RenderingModule::AllocateScene() // NOLINT
    {
        return new Scene();
    }

    void RenderingModule::DestroyScene(Render::IScene* inScene) // NOLINT
    {
        delete inScene;
    }

    void RenderingModule::ShutdownRenderingThread()
    {
        renderingThread = nullptr;
    }

    void RenderingModule::FlushAllRenderingCommands() const
    {
        Assert(renderingThread != nullptr);
        renderingThread->Flush();
    }
}

IMPLEMENT_MODULE(RENDERING_API, Rendering::RenderingModule);
