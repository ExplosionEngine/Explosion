//
// Created by johnk on 2025/1/16.
//

#include <ranges>

#include <Core/Console.h>

namespace Core {
    ConsoleSetting::ConsoleSetting(const std::string& inName, const std::string& inDescription, const CSFlags& inFlags)
        : name(inName)
        , description(inDescription)
        , flags(inFlags)
    {
        Console::Get().RegisterConsoleSetting(*this);
    }

    ConsoleSetting::~ConsoleSetting() = default;

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
}
