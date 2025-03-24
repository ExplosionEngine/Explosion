//
// Created by johnk on 2025/3/4.
//

#pragma once

#include <Common/Math/Rect.h>
#include <Common/Math/Projection.h>
#include <Render/RenderModule.h>
#include <Render/View.h>
#include <Runtime/Component/Transform.h>
#include <Runtime/Component/Camera.h>
#include <Runtime/ECS.h>
#include <Runtime/Client.h>
#include <Runtime/Api.h>

namespace Runtime {
    class RUNTIME_API EClass() RenderSystem final : public System {
        EPolyClassBody(RenderSystem)

    public:
        explicit RenderSystem(ECRegistry& inRegistry, const SystemSetupContext& inContext);
        ~RenderSystem() override;

        NonCopyable(RenderSystem)
        NonMovable(RenderSystem)

        void Tick(float inDeltaTimeSeconds) override;

    private:
        static Common::URect GetPlayerViewport(uint32_t inWidth, uint32_t inHeight, uint8_t inPlayerNum, uint8_t inPlayerIndex);
        template <typename T> Render::View BuildViewForPlayer(Entity inEntity, uint8_t inPlayerNum, uint8_t inPlayerIndex) const;
        std::vector<Render::View> BuildViews() const;

        Render::RenderModule& renderModule;
        PlayType playType;
        Client* client;
        RHI::Fence* lastFrameFence;
    };
}

namespace Runtime {
    template <typename T>
    Render::View RenderSystem::BuildViewForPlayer(Entity inEntity, uint8_t inPlayerNum, uint8_t inPlayerIndex) const
    {
        const auto& player = registry.Get<T>(inEntity);
        const auto& camera = registry.Get<Camera>(inEntity);
        const auto& worldTransform = registry.Get<WorldTransform>(inEntity);
        const auto& clientViewport = client->GetViewport();

        const auto width = clientViewport.GetWidth();
        const auto height = clientViewport.GetHeight();

        Render::View view = renderModule.CreateView();
        view.state = player.viewState;
        view.data.viewport = GetPlayerViewport(width, height, inPlayerNum, inPlayerIndex);
        view.data.viewMatrix = worldTransform.localToWorld.GetTransformMatrixNoScale().Inverse();
        view.data.origin = worldTransform.localToWorld.translation;
        if (camera.perspective) {
            const Common::FReversedZPerspectiveProjection projection(camera.fov.value(), static_cast<float>(width), static_cast<float>(height), camera.nearPlane, camera.farPlane);
            view.data.projectionMatrix = projection.GetProjectionMatrix();
        } else {
            const Common::FReversedZOrthoProjection projection(static_cast<float>(width), static_cast<float>(height), camera.nearPlane, camera.farPlane);
            view.data.projectionMatrix = projection.GetProjectionMatrix();
        }
        return view;
    }
}
