//
// Created by johnk on 2022/8/3.
//

#pragma once

#include <Render/Scene.h>
#include <Render/View.h>

namespace RHI {
    class Texture;
    class Semaphore;
    class Fence;
}

namespace Render {
    class Renderer {
    public:
        struct Params {
            const Scene* scene;
            const RHI::Texture* surface;
            std::vector<const View*> views;
            RHI::Semaphore* waitSemaphore;
            RHI::Semaphore* signalSemaphore;
            RHI::Fence* signalFence;
        };

        explicit Renderer(const Params& inParams);
        virtual ~Renderer();

        virtual void Render(float inDeltaTimeSeconds) = 0;

    protected:
        const Scene* scene;
        const RHI::Texture* surface;
        std::vector<const View*> views;
        RHI::Semaphore* waitSemaphore;
        RHI::Semaphore* signalSemaphore;
        RHI::Fence* signalFence;
    };

    class StandardRenderer final : public Renderer {
    public:
        explicit StandardRenderer(const Params& inParams);
        ~StandardRenderer() override;

        void Render(float inDeltaTimeSeconds) override;

    private:
    };

    // TODO ScriptableRenderer
}
