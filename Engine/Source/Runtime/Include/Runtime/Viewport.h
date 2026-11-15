#pragma once

#include <unordered_map>

#include <Render/RenderSurface.h>
#include <Runtime/Client.h>
#include <Runtime/ECS.h>
#include <Runtime/RenderThreadPtr.h>

namespace Runtime {
    class RUNTIME_API Viewport {
    public:
        virtual ~Viewport();

        NonCopyable(Viewport)
        NonMovable(Viewport)

        virtual void Render(float inDeltaTimeSeconds);
        void WaitIdle() const;

    protected:
        Viewport();

        virtual Client& GetClient() = 0;
        virtual Render::RenderSurface& GetRenderSurface() = 0;
        virtual std::vector<Render::View> BuildViews(ECRegistry& inRegistry, Common::UVec2 inSize) = 0;
        Render::View BuildViewForCamera(ECRegistry& inRegistry, Entity inEntity, const Common::URect& inRect);

    private:
        Render::RenderModule& renderModule;
        Common::UniquePtr<RHI::Fence> frameFence;
        std::unordered_map<Entity, RenderThreadPtr<Render::ViewState>> viewStates;
    };
}
