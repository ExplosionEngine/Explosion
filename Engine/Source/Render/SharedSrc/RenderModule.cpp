//
// Created by johnk on 2023/8/4.
//

#include <Render/RenderModule.h>
#include <Render/Scene.h>

namespace Render {
    RenderModule::RenderModule()
        : initialized(false)
        , rhiInstance(nullptr)
    {
    }

    RenderModule::~RenderModule() = default;

    void RenderModule::OnLoad()
    {
        Module::OnLoad();
    }

    void RenderModule::OnUnload()
    {
        Module::OnUnload();
    }

    Core::ModuleType RenderModule::Type() const
    {
        return Core::ModuleType::mDynamic;
    }

    void RenderModule::Initialize(const RenderModuleInitParams& inParams)
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

    RHI::Device* RenderModule::GetDevice() const
    {
        return rhiDevice.Get();
    }

    Render::Scene* RenderModule::AllocateScene() // NOLINT
    {
        return new Scene();
    }

    void RenderModule::DestroyScene(Render::Scene* inScene) // NOLINT
    {
        delete inScene;
    }

    void RenderModule::ShutdownRenderingThread()
    {
        renderingThread = nullptr;
    }

    void RenderModule::FlushAllRenderingCommands() const
    {
        Assert(renderingThread != nullptr);
        renderingThread->Flush();
    }
}

IMPLEMENT_DYNAMIC_MODULE(RENDER_API, Render::RenderModule);
