//
// Created by johnk on 2023/8/4.
//

#pragma once

#include <Common/Memory.h>
#include <Common/Concurrent.h>
#include <Common/Debug.h>
#include <Core/Module.h>
#include <Render/Scene.h>
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
        Scene* AllocateScene();
        void ShutdownRenderingThread();
        void FlushAllRenderingCommands() const;

        template <typename F, typename... Args>
        auto EnqueueRenderingCommand(F&& command, Args&&... args)
        {
            Assert(renderingThread != nullptr);
            return renderingThread->EmplaceTask(std::forward<F>(command), std::forward<Args>(args)...);
        }

    private:
        bool initialized;
        Common::UniqueRef<Common::WorkerThread> renderingThread;
        RHI::Instance* rhiInstance;
        Common::UniqueRef<RHI::Device> rhiDevice;
    };
}
