//
// Created by johnk on 2023/7/31.
//

#include <Common/Debug.h>
#include <Core/Paths.h>

namespace Core {
    Common::Path Paths::executablePath = Common::Path();
    Common::Path Paths::workingDir = Common::Path();
    Common::Path Paths::currentProjectFile = Common::Path();

    void Paths::SetExecutableDir(const Common::Path& inPath)
    {
        executablePath = inPath;
    }

    void Paths::SetCurrentProjectFile(const Common::Path& inFile)
    {
        currentProjectFile = inFile;
    }

    bool Paths::HasSetProjectFile()
    {
        return !currentProjectFile.Empty();
    }

    bool Paths::HasSetExecutableDir()
    {
        return !executablePath.Empty();
    }

    Common::Path Paths::WorkingDir()
    {
        // working directory is set to engine binaries directory as default
        if (workingDir.Empty()) {
            workingDir = Common::Path::WorkingDirectory();
        }
        return workingDir;
    }

    Common::Path Paths::ExecutablePath()
    {
        Assert(!executablePath.Empty());
        return executablePath;
    }

    Common::Path Paths::EngineRootDir()
    {
#if BUILD_TEST
        if (HasSetExecutableDir()) {
            return ExecutablePath().Parent().Parent();
        }
        return WorkingDir().Parent();
#else
        Assert(HasSetExecutableDir());
        return ExecutableDir().Parent().Parent();
#endif
    }

    Common::Path Paths::EngineResDir()
    {
        return EngineRootDir() / "Resource";
    }

    Common::Path Paths::EngineShaderDir()
    {
        return EngineRootDir() / "Shader";
    }

    Common::Path Paths::EngineAssetDir()
    {
        return EngineRootDir() / "Asset";
    }

    Common::Path Paths::EngineBinDir()
    {
        return EngineRootDir() / "Binaries";
    }

    Common::Path Paths::EngineCacheDir()
    {
        return EngineRootDir() / "Cache";
    }

    Common::Path Paths::EngineLogDir()
    {
        return EngineCacheDir() / "Log";
    }

    Common::Path Paths::EnginePluginDir()
    {
        return EngineRootDir() / "Plugin";
    }

    Common::Path Paths::EnginePluginAssetDir(const std::string& pluginName)
    {
        return EnginePluginDir() / pluginName / "Asset";
    }

    Common::Path Paths::ProjectFile()
    {
        return currentProjectFile;
    }

    Common::Path Paths::ProjectRootDir()
    {
        return currentProjectFile.Parent();
    }

    Common::Path Paths::ProjectAssetDir()
    {
        return ProjectRootDir() / "Asset";
    }

    Common::Path Paths::ProjectBinDir()
    {
        return ProjectRootDir() / "Binaries";
    }

    Common::Path Paths::ProjectCacheDir()
    {
        return ProjectRootDir() / "Cache";
    }

    Common::Path Paths::ProjectLogDir()
    {
        return ProjectCacheDir() / "Log";
    }

    Common::Path Paths::ProjectPluginDir()
    {
        return ProjectRootDir() / "Plugin";
    }

    Common::Path Paths::ProjectPluginAssetDir(const std::string& pluginName)
    {
        return ProjectPluginDir() / pluginName / "Asset";
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
        return EngineRootDir() / "Test";
    }
#endif
}
