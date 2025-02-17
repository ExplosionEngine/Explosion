//
// Created by johnk on 2025/2/17.
//

#pragma once

#include <string>
#include <unordered_set>

#include <Mirror/Meta.h>
#include <Runtime/Api.h>

namespace Runtime {
    class RUNTIME_API EClass() Project { // NOLINT
        EClassBody(Project)

    public:
        Project();

        EFunc() void SetName(const std::string& inName);
        EFunc() void SetDescription(const std::string& inDescription);
        EFunc() void EnablePlugin(const std::string& inPluginName);
        EFunc() void DisablePlugin(const std::string& inPluginName);
        EFunc() const std::string& GetName() const;
        EFunc() const std::string& Description() const;
        EFunc() const std::unordered_set<std::string>& GetPlugins() const;

    private:
        EProperty() std::string name;
        EProperty() std::string description;
        EProperty() std::unordered_set<std::string> plugins;
    };
}
