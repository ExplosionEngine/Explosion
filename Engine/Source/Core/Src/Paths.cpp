//
// Created by johnk on 2023/7/31.
//

#include <Core/Paths.h>

namespace Core {
    std::filesystem::path Paths::workdingDir = std::filesystem::path();
    std::filesystem::path Paths::currentProjectFile = std::filesystem::path();

    std::filesystem::path Paths::WorkingDir()
    {
        // working directory is set to engine binaries directory as default
        if (workdingDir.empty()) {
            workdingDir = std::filesystem::current_path();
        }
        return workdingDir;
    }

    std::filesystem::path Paths::EngineRoot()
    {
        return WorkingDir().parent_path();
    }

    std::filesystem::path Paths::EngineShaderPath()
    {
        return EngineRoot() / "Shader";
    }

    std::filesystem::path Paths::EngineAssetPath()
    {
        return EngineRoot() / "Asset";
    }

    std::filesystem::path Paths::EngineBinariesPath()
    {
        return EngineRoot() / "Binaries";
    }

    std::filesystem::path Paths::EnginePluginPath()
    {
        return EngineRoot() / "Plugin";
    }

    std::filesystem::path Paths::EnginePluginAssetPath(const std::string& pluginName)
    {
        return EnginePluginPath() / pluginName / "Asset";
    }

    void Paths::SetCurrentProjectFile(std::filesystem::path inFile)
    {
        currentProjectFile = std::move(inFile);
    }

    std::filesystem::path Paths::ProjectFile()
    {
        return currentProjectFile;
    }

    std::filesystem::path Paths::ProjectRoot()
    {
        return currentProjectFile.parent_path();
    }

    std::filesystem::path Paths::ProjectAssetPath()
    {
        return ProjectRoot() / "Asset";
    }

    std::filesystem::path Paths::ProjectBinariesPath()
    {
        return ProjectRoot() / "Binaries";
    }

    std::filesystem::path Paths::ProjectPluginPath()
    {
        return ProjectRoot() / "Plugin";
    }

    std::filesystem::path Paths::ProjectPluginAssetPath(const std::string& pluginName)
    {
        return ProjectPluginPath() / pluginName / "Asset";
    }

#if BUILD_TEST
    std::filesystem::path Paths::EngineTestPath()
    {
        return EngineRoot() / "Test";
    }
#endif
}
