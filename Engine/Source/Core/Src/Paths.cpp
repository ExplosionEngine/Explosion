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
}
