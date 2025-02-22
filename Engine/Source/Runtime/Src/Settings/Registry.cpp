//
// Created by johnk on 2025/2/21.
//

#include <Runtime/Settings/Registry.h>

namespace Runtime::Internal {
    static Common::Path GetConfigPathForSettings(SettingsClass inClass)
    {
        // TODO order: game -> engine
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

    void SettingsRegistry::SaveSettingsDyn(SettingsClass inClass)
    {
        // TODO
    }

    void SettingsRegistry::LoadSettingsDyn(SettingsClass inClass)
    {
        // TODO
    }

    void SettingsRegistry::SaveAllSettings()
    {
        for (const auto* clazz : settingsMap | std::views::keys) {
            SaveSettingsDyn(clazz);
        }
    }

    void SettingsRegistry::LoadAllSettings()
    {
        for (const auto* clazz : settingsMap | std::views::keys) {
            LoadSettingsDyn(clazz);
        }
    }

    void SettingsRegistry::RegisterInternalSettings()
    {
        // TODO
    }
}
