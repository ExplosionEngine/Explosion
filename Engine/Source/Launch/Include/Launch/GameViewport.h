#pragma once

#include <Launch/GameClient.h>
#include <Runtime/Viewport.h>

namespace Launch {
    class GameViewport final : public Runtime::Viewport {
    public:
        GameViewport(GameClient& inClient, Render::RenderSurface& inSurface);
        ~GameViewport() override;

    private:
        Runtime::Client& GetClient() override;
        Render::RenderSurface& GetRenderSurface() override;
        std::vector<Render::View> BuildViews(Runtime::ECRegistry& inRegistry, Common::UVec2 inSize) override;
        static Common::URect GetPlayerViewport(Common::UVec2 inSize, uint8_t inPlayerNum, uint8_t inPlayerIndex);

        GameClient& client;
        Render::RenderSurface& surface;
    };
}
