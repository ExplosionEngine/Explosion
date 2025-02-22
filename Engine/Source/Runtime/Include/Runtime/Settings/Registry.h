//
// Created by johnk on 2025/2/21.
//

#pragma once

#include <Mirror/Meta.h>
#include <Mirror/Mirror.h>
#include <Runtime/Api.h>

namespace Runtime {
    using SettingsClass = const Mirror::Class*;

    class RUNTIME_API SettingsRegistry {
    public:
        EClassBody(SettingsRegistry)

        EFunc() static SettingsRegistry& Get();

        ~SettingsRegistry();

        template <typename T> void RegisterSettings();
        template <typename T> T& GetSettings();
        template <typename T> void SaveSettings();
        template <typename T> void LoadSettings();

        EFunc() void RegisterSettingsDyn(SettingsClass inClass);
        EFunc() Mirror::Any GetSettingsDyn(SettingsClass inClass);
        EFunc() void SaveSettingsDyn(SettingsClass inClass);
        EFunc() void LoadSettingsDyn(SettingsClass inClass);
        EFunc() void SaveAllSettings();
        EFunc() void LoadAllSettings();

    private:
        SettingsRegistry();

        void RegisterInternalSettings();

        std::unordered_map<SettingsClass, Mirror::Any> settingsMap;
    };
}

namespace Runtime {
    template <typename T>
    void SettingsRegistry::RegisterSettings()
    {
        RegisterSettingsDyn(&Mirror::Class::Get<T>());
    }

    template <typename T>
    T& SettingsRegistry::GetSettings()
    {
        return GetSettingsDyn(&Mirror::Class::Get<T>()).template As<T&>();
    }

    template <typename T>
    void SettingsRegistry::SaveSettings()
    {
        SaveSettingsDyn(&Mirror::Class::Get<T>());
    }

    template <typename T>
    void SettingsRegistry::LoadSettings()
    {
        LoadSettingsDyn(&Mirror::Class::Get<T>());
    }
}
