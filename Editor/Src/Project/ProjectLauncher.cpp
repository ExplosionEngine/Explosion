#include <Editor/Project/ProjectLauncher.h>

namespace Editor {
    ProjectLauncher::ProjectLauncher(EditorLaunchOptions inOptions, LaunchProcess inLaunchProcess)
        : options(std::move(inOptions))
        , launchProcess(std::move(inLaunchProcess))
    {
    }

    Common::Result<void, std::string> ProjectLauncher::Launch(const Common::Path& inProjectPath) const
    {
        std::vector<std::string> arguments { "-project", inProjectPath.String(), "-rhi", options.rhiType };
        if (options.gpuDebug) {
            arguments.emplace_back("-gpuDebug");
        }
        if (options.softwareGpu) {
            arguments.emplace_back("-softwareGpu");
        }
        return launchProcess(options.executablePath, arguments);
    }
}
