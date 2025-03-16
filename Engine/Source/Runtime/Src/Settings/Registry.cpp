//
// Created by johnk on 2025/2/21.
//

#include <Common/File.h>
#include <Core/Paths.h>
#include <Runtime/Settings/Registry.h>
#include <Runtime/Settings/Game.h>

namespace Runtime::Internal {
    static Common::Path GetConfigPathForSettings(SettingsClass inClass)
    {
        return Core::Paths::GameConfigDir() / std::format("{}.json", inClass->GetName());
    }
}

namespace Runtime {
    SettingsRegistry& SettingsRegistry::Get()
    {
        static SettingsRegistry instance;
        return instance;
    }

    SettingsRegistry::SettingsRegistry()
    {
        RegisterInternalSettings();
    }

    SettingsRegistry::~SettingsRegistry() = default;

    void SettingsRegistry::RegisterSettingsDyn(SettingsClass inClass)
    {
        settingsMap.emplace(inClass, inClass->Construct());
    }

    Mirror::Any SettingsRegistry::GetSettingsDyn(SettingsClass inClass)
    {
        AssertWithReason(settingsMap.contains(inClass), "did you forget to register settings in your module ?");
        return settingsMap.at(inClass);
    }

    void SettingsRegistry::LoadSettingsDyn(SettingsClass inClass)
    {
        const auto configPath = Internal::GetConfigPathForSettings(inClass);
        if (!configPath.Exists()) {
            return;
        }

        const rapidjson::Document document = Common::FileUtils::ReadJsonFile(configPath.String());
        Assert(document.IsObject());
        settingsMap.at(inClass).JsonDeserialize(document);
    }

    void SettingsRegistry::LoadAllSettings()
    {
        for (const auto* clazz : settingsMap | std::views::keys) {
            LoadSettingsDyn(clazz);
        }
    }

    void SettingsRegistry::SaveSettingsDyn(SettingsClass inClass) const
    {
#if !BUILD_EDITOR
        Assert(!inClass->GetMetaBoolOr(MetaPresets::gameReadOnly, false));
#endif

        rapidjson::Document document;
        settingsMap.at(inClass).JsonSerialize(document, document.GetAllocator());

        const auto configPath = Internal::GetConfigPathForSettings(inClass);
        Common::FileUtils::WriteJsonFile(configPath.String(), document);
    }

    void SettingsRegistry::SaveAllSettings() const
    {
        for (const auto* clazz : settingsMap | std::views::keys) {
            SaveSettingsDyn(clazz);
        }
    }

    void SettingsRegistry::RegisterInternalSettings()
    {
        RegisterSettings<GameSettings>();
    }
}
