#include <Launch/GameViewport.h>
#include <Runtime/Component/Camera.h>
#include <Runtime/Component/Player.h>
#include <Runtime/Component/Transform.h>

namespace Launch {
    GameViewport::GameViewport(GameClient& inClient, Render::RenderSurface& inSurface)
        : client(inClient)
        , surface(inSurface)
    {
    }

    GameViewport::~GameViewport() = default;

    Runtime::Client& GameViewport::GetClient()
    {
        return client;
    }

    Render::RenderSurface& GameViewport::GetRenderSurface()
    {
        return surface;
    }

    std::vector<Render::View> GameViewport::BuildViews(Runtime::ECRegistry& inRegistry, Common::UVec2 inSize)
    {
        std::vector<Render::View> views;
        const auto cameras = inRegistry.View<Runtime::Camera, Runtime::WorldTransform>();
        for (const auto& entry : cameras.All()) {
            const auto entity = std::get<0>(entry);
            auto rect = Common::URect::FromMinExtent(0, 0, inSize.x, inSize.y);
            if (inRegistry.Has<Runtime::LocalPlayer>(entity)) {
                const auto& players = inRegistry.GGet<Runtime::PlayersInfo>().players;
                rect = GetPlayerViewport(inSize, static_cast<uint8_t>(players.size()), inRegistry.Get<Runtime::LocalPlayer>(entity).localPlayerIndex);
            }
            views.emplace_back(BuildViewForCamera(inRegistry, entity, rect));
        }
        return views;
    }

    Common::URect GameViewport::GetPlayerViewport(Common::UVec2 inSize, uint8_t inPlayerNum, uint8_t inPlayerIndex)
    {
        Assert(inPlayerNum > 0 && inPlayerIndex < inPlayerNum);
        if (inPlayerNum == 1) {
            return Common::URect::FromMinExtent(0, 0, inSize.x, inSize.y);
        }
        if (inPlayerNum == 2) {
            return Common::URect::FromMinExtent(inSize.x / 2 * inPlayerIndex, 0, inSize.x / 2, inSize.y);
        }
        Assert(inPlayerNum <= 4);
        return Common::URect::FromMinExtent(
            inSize.x / 2 * (inPlayerIndex % 2), inSize.y / 2 * (inPlayerIndex / 2), inSize.x / 2, inSize.y / 2);
    }
}
