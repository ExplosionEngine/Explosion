//
// Created by johnk on 2025/3/13.
//

#include <Runtime/System/Player.h>
#include <Runtime/Settings/Game.h>
#include <Runtime/Settings/Registry.h>
#include <Runtime/Engine.h>

namespace Runtime {
    PlayerSystem::PlayerSystem(ECRegistry& inRegistry, const SystemSetupContext& inContext)
        : System(inRegistry, inContext)
        , activeLocalPlayerNum(0)
        , renderModule(EngineHolder::Get().GetRenderModule())
    {
        auto& playersInfo = registry.GEmplace<PlayersInfo>();

#if BUILD_EDITOR
        if (inContext.playType == PlayType::editor) {
            playersInfo.players.emplace_back(CreatePlayer<EditorPlayer>());
        }
#endif
        if (inContext.playType == PlayType::game) {
            const auto& gameSettings = SettingsRegistry::Get().GetSettings<GameSettings>();
            playersInfo.players.resize(gameSettings.initialLocalPlayerNum);
            for (auto& playerEntity : playersInfo.players) {
                playerEntity = CreatePlayer<LocalPlayer>();
            }
        }
    }

    PlayerSystem::~PlayerSystem()
    {
        registry.View<LocalPlayer>().Each([this](Entity e, LocalPlayer& player) -> void { FinalizePlayer<LocalPlayer>(e); });
#if BUILD_EDITOR
        registry.View<EditorPlayer>().Each([this](Entity e, EditorPlayer& player) -> void { FinalizePlayer<EditorPlayer>(e); });
#endif
    }
} // namespace Runtime
