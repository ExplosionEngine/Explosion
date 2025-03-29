//
// Created by johnk on 2025/3/13.
//

#pragma once

#include <Render/RenderModule.h>
#include <Runtime/Api.h>
#include <Runtime/Component/Camera.h>
#include <Runtime/Component/Player.h>
#include <Runtime/Component/Transform.h>
#include <Runtime/ECS.h>
#include <Runtime/Meta.h>

namespace Runtime {
    class RUNTIME_API EClass() PlayerSystem final : public System {
        EClassBody(PlayerSystem)

    public:
        NonCopyable(PlayerSystem)
        NonMovable(PlayerSystem)

        PlayerSystem(ECRegistry& inRegistry, const SystemSetupContext& inContext);
        ~PlayerSystem() override;

    private:
        template <typename T> Entity CreatePlayer();
        template <typename T> void CreatePlayerSpecialPart(T& inPlayer);

        uint8_t activeLocalPlayerNum;
        Render::RenderModule& renderModule;
    };
}

namespace Runtime {
    template <typename T>
    Entity PlayerSystem::CreatePlayer()
    {
        const auto& playerStartQuery = registry.View<PlayerStart, WorldTransform>().All();
        Assert(playerStartQuery.size() == 1);
        const auto& [playerStartEntity, playerStart, playerStartTransform] = playerStartQuery[0];

        const auto playerEntity = registry.Create();
        registry.Emplace<Camera>(playerEntity);
        registry.Emplace<WorldTransform>(playerEntity, playerStartTransform);

        auto& player = registry.Emplace<T>(playerEntity);
        player.viewState = renderModule.NewViewState();
        CreatePlayerSpecialPart<T>(player);
        return playerEntity;
    }

    template <typename T>
    inline void PlayerSystem::CreatePlayerSpecialPart(T& inPlayer) // NOLINT
    {
        Unimplement();
    }

    template <>
    inline void PlayerSystem::CreatePlayerSpecialPart<LocalPlayer>(LocalPlayer& inPlayer) // NOLINT
    {
        inPlayer.localPlayerIndex = activeLocalPlayerNum++;
    }

#if BUILD_EDITOR
    template <>
    inline void PlayerSystem::CreatePlayerSpecialPart<EditorPlayer>(EditorPlayer& inPlayer) // NOLINT
    {
    }
#endif
} // namespace Runtime
