//
// Created by johnk on 2023/8/4.
//

#include <Core/Thread.h>
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

        RenderThread::Get().Start();
        RenderWorkerThreads::Get().Start();

        rhiInstance = RHI::Instance::GetByType(inParams.rhiType);
        rhiDevice = rhiInstance->GetGpu(0)->RequestDevice(
            RHI::DeviceCreateInfo()
                .AddQueueRequest(RHI::QueueRequestInfo(RHI::QueueType::graphics, 1))
                .AddQueueRequest(RHI::QueueRequestInfo(RHI::QueueType::compute, 1))
                .AddQueueRequest(RHI::QueueRequestInfo(RHI::QueueType::transfer, 1)));

        initialized = true;
    }

    void RenderModule::DeInitialize()
    {
        RenderThread::Get().Stop();
        RenderWorkerThreads::Get().Stop();

        rhiInstance = nullptr;
        rhiDevice = nullptr;
        initialized = false;
    }

    RHI::Device* RenderModule::GetDevice() const
    {
        return rhiDevice.Get();
    }

    Render::RenderThread& RenderModule::GetRenderThread() const // NOLINT
    {
        return RenderThread::Get();
    }

    Common::UniqueRef<Render::Scene> RenderModule::NewScene() // NOLINT
    {
        return new Scene();
    }

    Common::UniqueRef<View> RenderModule::NewView() // NOLINT
    {
        return new View();
    }
}

IMPLEMENT_DYNAMIC_MODULE(RENDER_API, Render::RenderModule);
