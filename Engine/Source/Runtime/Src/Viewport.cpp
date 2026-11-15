#include <Common/Math/Projection.h>
#include <Common/Math/View.h>
#include <Runtime/Component/Camera.h>
#include <Runtime/Component/Scene.h>
#include <Runtime/Component/Transform.h>
#include <Runtime/Viewport.h>
#include <Runtime/World.h>

namespace Runtime {
    Viewport::Viewport()
        : renderModule(EngineHolder::Get().GetRenderModule())
        , frameFence(renderModule.GetDevice()->CreateFence(true))
    {
    }

    Viewport::~Viewport()
    {
        WaitIdle();
    }

    void Viewport::Render(float inDeltaTimeSeconds)
    {
        auto& world = GetClient().GetWorld();
        auto& surface = GetRenderSurface();
        if (world.Stopped()) {
            return;
        }
        auto& registry = world.ecRegistry;
        std::erase_if(viewStates, [&](const auto& inEntry) -> bool {
            return !registry.Valid(inEntry.first) || !registry.Has<Camera>(inEntry.first);
        });
        renderModule.GetRenderThread().EmplaceTask(
            [
                fence = frameFence.Get(),
                views = BuildViews(registry, surface.GetSize()),
                scene = registry.GGet<SceneHolder>().scene.Get(),
                surface = &surface,
                renderModule = &renderModule,
                inDeltaTimeSeconds
            ]() -> void {
                fence->Reset();
                const auto target = surface->GetRenderTarget();
                Render::StandardRenderer::Params params;
                params.device = renderModule->GetDevice();
                params.scene = scene;
                params.surface = target.texture;
                params.surfaceExtent = surface->GetSize();
                params.surfaceBeforeRenderState = target.beforeRenderState;
                params.surfaceAfterRenderState = target.afterRenderState;
                params.views = views;
                params.waitSemaphore = target.waitSemaphore;
                params.signalSemaphore = target.signalSemaphore;
                params.signalFence = fence;

                auto renderer = renderModule->CreateStandardRenderer(params);
                renderer->Render(inDeltaTimeSeconds);
                // Renderer-owned command buffers and transient resources must outlive the submitted GPU work.
                fence->Wait();
            });
    }

    void Viewport::WaitIdle() const
    {
        renderModule.GetRenderThread().Flush();
        const auto fence = renderModule.GetDevice()->CreateFence(false);
        renderModule.GetDevice()->GetQueue(RHI::QueueType::graphics, 0)->Flush(fence.Get());
        fence->Wait();
    }

    Render::View Viewport::BuildViewForCamera(ECRegistry& inRegistry, Entity inEntity, const Common::URect& inRect)
    {
        const auto& cameraComponent = inRegistry.Get<Camera>(inEntity);
        const auto& transform = inRegistry.Get<WorldTransform>(inEntity).localToWorld;
        auto& state = viewStates[inEntity];
        if (state == nullptr) {
            state = renderModule.NewViewState();
        }

        Render::View view = renderModule.CreateView();
        view.state = state.Get();
        view.data.viewport = inRect;
        view.data.viewMatrix = Common::FViewTransform(transform).GetViewMatrix();
        view.data.origin = transform.translation;
        if (cameraComponent.perspective) {
            const Common::FReversedZPerspectiveProjection projection(
                cameraComponent.fov.value(), static_cast<float>(inRect.ExtentX()), static_cast<float>(inRect.ExtentY()),
                cameraComponent.nearPlane, cameraComponent.farPlane);
            view.data.projectionMatrix = projection.GetProjectionMatrix();
        } else {
            const Common::FReversedZOrthoProjection projection(
                static_cast<float>(inRect.ExtentX()), static_cast<float>(inRect.ExtentY()), cameraComponent.nearPlane, cameraComponent.farPlane);
            view.data.projectionMatrix = projection.GetProjectionMatrix();
        }
        return view;
    }

}
