//
// Created by johnk on 2025/3/4.
//

#include <Render/Renderer.h>
#include <Runtime/Component/Player.h>
#include <Runtime/Component/Scene.h>
#include <Runtime/Engine.h>
#include <Runtime/System/Render.h>

namespace Runtime {
    RenderSystem::RenderSystem(ECRegistry& inRegistry, const SystemSetupContext& inContext)
        : System(inRegistry, inContext)
        , renderModule(EngineHolder::Get().GetRenderModule())
        , playType(inContext.playType)
        , client(inContext.client)
        , lastFrameFence(renderModule.GetDevice()->CreateFence(true).Release())
    {
    }

    RenderSystem::~RenderSystem() // NOLINT
    {
        renderModule.GetRenderThread().EmplaceTask([fence = lastFrameFence]() -> void {
            fence->Wait();
            delete fence;
        });
    }

    void RenderSystem::Tick(float inDeltaTimeSeconds)
    {
        auto& clientViewport = client->GetViewport();
        renderModule.GetRenderThread().EmplaceTask(
            [
                fence = lastFrameFence,
                views = BuildViews(),
                scene = registry.GGet<SceneHolder>().scene.Get(),
                surfaceExtent = Common::UVec2(clientViewport.GetWidth(), clientViewport.GetHeight()),
                presentInfo = clientViewport.GetNextPresentInfo(),
                renderModule = &renderModule,
                inDeltaTimeSeconds
            ]() -> void {
                fence->Wait();
                fence->Reset();

                Render::StandardRenderer::Params rendererParams;
                rendererParams.device = renderModule->GetDevice();
                rendererParams.scene = scene;
                rendererParams.surface = presentInfo.backTexture;
                rendererParams.surfaceExtent = surfaceExtent;
                rendererParams.views = views;
                rendererParams.waitSemaphore = presentInfo.imageReadySemaphore;
                rendererParams.signalSemaphore = presentInfo.renderFinishedSemaphore;
                rendererParams.signalFence = fence;

                auto renderer = renderModule->CreateStandardRenderer(rendererParams);
                renderer.Render(inDeltaTimeSeconds);
            });
    }

    Common::URect RenderSystem::GetPlayerViewport(uint32_t inWidth, uint32_t inHeight, uint8_t inPlayerNum, uint8_t inPlayerIndex)
    {
        Assert(inPlayerNum > 0 && inPlayerIndex < inPlayerNum);
        if (inPlayerNum == 1) {
            return { 0, 0, inWidth, inHeight };
        }
        if (inPlayerNum == 2) {
            const auto widthPerPlayer = inWidth / 2;
            return { widthPerPlayer * inPlayerIndex, 0, widthPerPlayer, inHeight };
        }
        if (inPlayerNum <= 4) {
            const auto widthPerPlayer = inWidth / 2;
            const auto heightPerPlayer = inHeight / 2;
            return { widthPerPlayer * (inPlayerIndex % 2), heightPerPlayer * (inPlayerIndex / 2), widthPerPlayer, heightPerPlayer };
        }
        Unimplement();
        return {};
    }

    std::vector<Render::View> RenderSystem::BuildViews() const
    {
        const auto& playersInfo = registry.GGet<PlayersInfo>();
        const auto playerNum = playersInfo.players.size();

        std::vector<Render::View> result;
        result.reserve(playerNum);

        for (auto i = 0; i < playerNum; i++) {
            if (registry.Has<LocalPlayer>(i)) {
                result.emplace_back(BuildViewForPlayer<LocalPlayer>(playersInfo.players[i], playerNum, i));
            }
    #if BUILD_EDITOR
            if (registry.Has<EditorPlayer>(i)) {
                result.emplace_back(BuildViewForPlayer<EditorPlayer>(playersInfo.players[i], playerNum, i));
            }
    #endif
        }
        return result;
    }
}
