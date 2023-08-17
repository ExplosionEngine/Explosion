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

namespace Rendering {
    class RENDERING_API RenderingModule : public Core::Module {
    public:
        RenderingModule();
        ~RenderingModule() override;

        Render::IScene* AllocateScene();
        void DestroyScene(Render::IScene* inScene);
        void StartupRenderingThread();
        void ShutdownRenderingThread();
        void FlushAllRenderingCommands();

        template <typename F, typename... Args>
        auto EnqueueRenderingCommand(F&& command, Args&&... args)
        {
            Assert(renderingThread != nullptr);
            return renderingThread->EmplaceTask(std::forward<F>(command), std::forward<Args>(args)...);
        }

    private:
        Common::UniqueRef<Common::WorkerThread> renderingThread;
    };
}

IMPLEMENT_MODULE(RENDERING_API, Rendering::RenderingModule);
