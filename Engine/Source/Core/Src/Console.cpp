//
// Created by johnk on 2025/1/16.
//

#include <ranges>
#include <vector>

#include <Common/File.h>
#include <Core/Console.h>

namespace Core::Internal {
    static void SetConsoleSettingFromJsonValue(ConsoleSetting& inConsoleSetting, const rapidjson::Value& inJsonValue)
    {
        if (inJsonValue.IsInt()) {
            inConsoleSetting.SetI32(inJsonValue.GetInt());
        } else if (inJsonValue.IsUint()) {
            inConsoleSetting.SetU32(inJsonValue.GetUint());
        } else if (inJsonValue.IsInt64()) {
            inConsoleSetting.SetI64(inJsonValue.GetInt64());
        } else if (inJsonValue.IsUint64()) {
            inConsoleSetting.SetU64(inJsonValue.GetUint64());
        } else if (inJsonValue.IsBool()) {
            inConsoleSetting.SetBool(inJsonValue.GetBool());
        } else if (inJsonValue.IsFloat()) {
            inConsoleSetting.SetFloat(inJsonValue.GetFloat());
        } else if (inJsonValue.IsDouble()) {
            inConsoleSetting.SetDouble(inJsonValue.GetDouble());
        } else if (inJsonValue.IsString()) {
            inConsoleSetting.SetString(std::string(inJsonValue.GetString(), inJsonValue.GetStringLength()));
        } else {
            Unimplement();
        }
    }
}

namespace Core {
    ConsoleSetting::ConsoleSetting(const std::string& inName, const std::string& inDescription, const CSFlags& inFlags)
        : name(inName)
        , description(inDescription)
        , flags(inFlags)
    {
        Console::Get().RegisterConsoleSetting(*this);
    }

    ConsoleSetting::~ConsoleSetting()
    {
        Console::Get().UnregisterConsoleSetting(*this);
    }

    const std::string& ConsoleSetting::Name() const
    {
        return name;
    }

    const std::string& ConsoleSetting::Description() const
    {
        return description;
    }

    CSFlags ConsoleSetting::Flags() const
    {
        return flags;
    }

    Console& Console::Get()
    {
        static Console instance;
        return instance;
    }

    Console::~Console() = default;

    bool Console::HasSetting(const std::string& inName) const
    {
        return settings.contains(inName);
    }

    ConsoleSetting* Console::FindSetting(const std::string& inName) const
    {
        const auto iter = settings.find(inName);
        return iter == settings.end() ? nullptr : iter->second;
    }

    ConsoleSetting& Console::GetSetting(const std::string& inName) const
    {
        return *settings.at(inName);
    }

    void Console::OverrideSettingsByConfig() const
    {
        std::vector<Common::Path> paths;
        paths.reserve(2);
        paths.emplace_back(Paths::EngineConfigDir() / "ConsoleSettings.json");
        if (Paths::HasSetGameRoot()) {
            paths.emplace_back(Paths::GameConfigDir() / "ConsoleSettings.json");
        }

        for (const auto& path : paths) {
            if (!path.Exists()) {
                continue;
            }

            rapidjson::Document document = Common::FileUtils::ReadJsonFile(path.String());
            Assert(document.IsObject());

            for (auto iter = document.MemberBegin(); iter != document.MemberEnd(); ++iter) {
                const auto& nameJson = iter->name;
                const auto nameString = std::string(nameJson.GetString(), nameJson.GetStringLength());
                if (!HasSetting(nameString)) {
                    continue;
                }

                auto& consoleSetting = GetSetting(nameString);
                if ((consoleSetting.Flags() & CSFlagBits::configOverridable) == CSFlags::null) {
                    continue;
                }

                Internal::SetConsoleSettingFromJsonValue(consoleSetting, iter->value);
            }
        }
    }

    void Console::PerformRenderThreadSettingsCopy() const
    {
        for (const auto& setting : settings | std::views::values) {
            setting->PerformRenderThreadCopy();
        }
    }

    Console::Console() = default;

    void Console::RegisterConsoleSetting(ConsoleSetting& inSetting)
    {
        settings.emplace(inSetting.Name(), &inSetting);
    }

    void Console::UnregisterConsoleSetting(ConsoleSetting& inSetting)
    {
        settings.erase(inSetting.Name());
    }
}
