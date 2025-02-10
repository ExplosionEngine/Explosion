//
// Created by johnk on 2023/8/4.
//

#pragma once

#include <Common/Memory.h>
#include <Common/Concurrent.h>
#include <Common/Debug.h>
#include <Core/Module.h>
#include <Render/Scene.h>
#include <Render/View.h>
#include <Render/Api.h>
#include <RHI/RHI.h>

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
        Common::UniqueRef<Scene> NewScene();
        Common::UniqueRef<View> NewView();
        void ShutdownRenderingThread();
        void FlushAllRenderingCommands() const;
        template <typename F, typename... Args> auto EnqueueRenderCommand(F&& command, Args&&... args);

    private:
        bool initialized;
        Common::UniqueRef<Common::WorkerThread> renderThread;
        RHI::Instance* rhiInstance;
        Common::UniqueRef<RHI::Device> rhiDevice;
    };
}

namespace Render {
    template <typename F, typename ... Args>
    auto RenderModule::EnqueueRenderCommand(F&& command, Args&&... args)
    {
        Assert(renderThread != nullptr);
        return renderThread->EmplaceTask(std::forward<F>(command), std::forward<Args>(args)...);
    }
}
