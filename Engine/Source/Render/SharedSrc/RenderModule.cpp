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

        renderingThread = Common::MakeUnique<Common::WorkerThread>("RenderingThread");
        renderingThread->EmplaceTask([]() -> void { Core::ThreadContext::SetTag(Core::ThreadTag::render); });

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
        renderingThread = nullptr;
        rhiInstance = nullptr;
        rhiDevice = nullptr;
        initialized = false;
    }

    RHI::Device* RenderModule::GetDevice() const
    {
        return rhiDevice.Get();
    }

    Common::UniqueRef<Render::Scene> RenderModule::NewScene() // NOLINT
    {
        return new Scene();
    }

    Common::UniqueRef<View> RenderModule::NewView() // NOLINT
    {
        return new View();
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
