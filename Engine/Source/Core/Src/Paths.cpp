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

    std::filesystem::path Paths::EngineRes()
    {
        return EngineRoot() / "Resource";
    }

    std::filesystem::path Paths::EngineShader()
    {
        return EngineRoot() / "Shader";
    }

    std::filesystem::path Paths::EngineAsset()
    {
        return EngineRoot() / "Asset";
    }

    std::filesystem::path Paths::EngineBin()
    {
        return EngineRoot() / "Binaries";
    }

    std::filesystem::path Paths::EnginePlugin()
    {
        return EngineRoot() / "Plugin";
    }

    std::filesystem::path Paths::EnginePluginAsset(const std::string& pluginName)
    {
        return EnginePlugin() / pluginName / "Asset";
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

    std::filesystem::path Paths::ProjectAsset()
    {
        return ProjectRoot() / "Asset";
    }

    std::filesystem::path Paths::ProjectBin()
    {
        return ProjectRoot() / "Binaries";
    }

    std::filesystem::path Paths::ProjectPlugin()
    {
        return ProjectRoot() / "Plugin";
    }

    std::filesystem::path Paths::ProjectPluginAsset(const std::string& pluginName)
    {
        return ProjectPlugin() / pluginName / "Asset";
    }

    std::filesystem::path Paths::EngineCMakeSourceDir()
    {
        return ENGINE_CMAKE_SOURCE_DIRECTORY;
    }

    std::filesystem::path Paths::EngineCMakeBinaryDir()
    {
        return ENGINE_CMAKE_BINARY_DIRECTORY;
    }

#if BUILD_TEST
    std::filesystem::path Paths::EngineTest()
    {
        return EngineRoot() / "Test";
    }
#endif
}
