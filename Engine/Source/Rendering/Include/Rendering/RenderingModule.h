//
// Created by johnk on 2023/8/4.
//

#pragma once

#include <Common/Memory.h>
#include <Common/Concurrent.h>
#include <Common/Debug.h>
#include <Core/Module.h>
#include <Render/Scene.h>
#include <Rendering/Api.h>
#include <RHI/RHI.h>

namespace Rendering {
    struct RenderingModuleInitParams {
        RHI::RHIType rhiType;
    };

    class RENDERING_API RenderingModule final : public Core::Module {
    public:
        RenderingModule();
        ~RenderingModule() override;

        void OnLoad() override;
        void OnUnload() override;
        Core::ModuleType Type() const override;

        void Initialize(const RenderingModuleInitParams& inParams);
        RHI::Device* GetDevice() const;
        Render::IScene* AllocateScene();
        void DestroyScene(Render::IScene* inScene);
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
