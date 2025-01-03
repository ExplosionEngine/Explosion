//
// Created by johnk on 2023/7/31.
//

#include <Core/Paths.h>

namespace Core {
    Common::Path Paths::executableDir = Common::Path();
    Common::Path Paths::workingDir = Common::Path();
    Common::Path Paths::currentProjectFile = Common::Path();

    void Paths::SetExecutableDir(const Common::Path& inPath)
    {
        executableDir = inPath;
    }

    void Paths::SetCurrentProjectFile(const Common::Path& inFile)
    {
        currentProjectFile = inFile;
    }

    bool Paths::HasProjectFile()
    {
        return !currentProjectFile.Empty();
    }

    bool Paths::HasSetExecutableDir()
    {
        return !executableDir.Empty();
    }

    Common::Path Paths::WorkingDir()
    {
        // working directory is set to engine binaries directory as default
        if (workingDir.Empty()) {
            workingDir = Common::Path::WorkingDirectory();
        }
        return workingDir;
    }

    Common::Path Paths::ExecutableDir()
    {
        Assert(!executableDir.Empty());
        return executableDir;
    }

    Common::Path Paths::EngineRoot()
    {
#if BUILD_TEST
        if (HasSetExecutableDir()) {
            return ExecutableDir().Parent().Parent();
        }
        return WorkingDir().Parent();
#else
        Assert(HasSetExecutableDir());
        return ExecutableDir().Parent().Parent();
#endif
    }

    Common::Path Paths::EngineRes()
    {
        return EngineRoot() / "Resource";
    }

    Common::Path Paths::EngineShader()
    {
        return EngineRoot() / "Shader";
    }

    Common::Path Paths::EngineAsset()
    {
        return EngineRoot() / "Asset";
    }

    Common::Path Paths::EngineBin()
    {
        return EngineRoot() / "Binaries";
    }

    Common::Path Paths::EnginePlugin()
    {
        return EngineRoot() / "Plugin";
    }

    Common::Path Paths::EnginePluginAsset(const std::string& pluginName)
    {
        return EnginePlugin() / pluginName / "Asset";
    }

    Common::Path Paths::ProjectFile()
    {
        return currentProjectFile;
    }

    Common::Path Paths::ProjectRoot()
    {
        return currentProjectFile.Parent();
    }

    Common::Path Paths::ProjectAsset()
    {
        return ProjectRoot() / "Asset";
    }

    Common::Path Paths::ProjectBin()
    {
        return ProjectRoot() / "Binaries";
    }

    Common::Path Paths::ProjectPlugin()
    {
        return ProjectRoot() / "Plugin";
    }

    Common::Path Paths::ProjectPluginAsset(const std::string& pluginName)
    {
        return ProjectPlugin() / pluginName / "Asset";
    }

    Common::Path Paths::EngineCMakeSourceDir()
    {
        return ENGINE_CMAKE_SOURCE_DIRECTORY;
    }

    Common::Path Paths::EngineCMakeBinaryDir()
    {
        return ENGINE_CMAKE_BINARY_DIRECTORY;
    }

#if BUILD_TEST
    Common::Path Paths::EngineTest()
    {
        return EngineRoot() / "Test";
    }
#endif
}
