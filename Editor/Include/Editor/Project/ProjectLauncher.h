#pragma once

#include <functional>
#include <string>
#include <vector>

#include <Common/FileSystem.h>
#include <Common/Process.h>
#include <Common/Result.h>

namespace Editor {
    struct EditorLaunchOptions {
        std::string executablePath;
        std::string rhiType;
        bool gpuDebug;
        bool softwareGpu;
    };

    class ProjectLauncher final {
    public:
        using LaunchProcess = std::function<Common::Result<void, std::string>(const std::string&, const std::vector<std::string>&)>;

        explicit ProjectLauncher(EditorLaunchOptions inOptions, LaunchProcess inLaunchProcess = Common::Process::Launch);

        Common::Result<void, std::string> Launch(const Common::Path& inProjectPath) const;

    private:
        EditorLaunchOptions options;
        LaunchProcess launchProcess;
    };
}
