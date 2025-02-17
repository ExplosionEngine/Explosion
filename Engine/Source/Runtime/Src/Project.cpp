//
// Created by johnk on 2025/2/17.
//

#include <Runtime/Project.h>

namespace Runtime {
    Project::Project() = default;

    void Project::SetName(const std::string& inName)
    {
        name = inName;
    }

    void Project::SetDescription(const std::string& inDescription)
    {
        description = inDescription;
    }

    void Project::EnablePlugin(const std::string& inPluginName)
    {
        plugins.emplace(inPluginName);
    }

    void Project::DisablePlugin(const std::string& inPluginName)
    {
        plugins.erase(inPluginName);
    }

    const std::string& Project::GetName() const
    {
        return name;
    }

    const std::string& Project::Description() const
    {
        return description;
    }

    const std::unordered_set<std::string>& Project::GetPlugins() const
    {
        return plugins;
    }
}
