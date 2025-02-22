//
// Created by johnk on 2023/7/31.
//

#include <Common/Debug.h>
#include <Core/Paths.h>

namespace Core {
    Common::Path Paths::executablePath = Common::Path();
    Common::Path Paths::workingDir = Common::Path();
    Common::Path Paths::gameRoot = Common::Path();

    void Paths::SetExecutableDir(const Common::Path& inPath)
    {
        executablePath = inPath;
    }

    void Paths::SetGameRoot(const Common::Path& inPath)
    {
        gameRoot = inPath;
    }

    bool Paths::HasSetExecutableDir()
    {
        return !executablePath.Empty();
    }

    bool Paths::HasSetGameRoot()
    {
        return !gameRoot.Empty();
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
        return WorkingDir().Parent();
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

    Common::Path Paths::EngineConfigDir()
    {
        return EngineRootDir() / "Config";
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

    Common::Path Paths::GameRootDir()
    {
        return gameRoot;
    }

    Common::Path Paths::GameAssetDir()
    {
        return GameRootDir() / "Asset";
    }

    Common::Path Paths::GameBinDir()
    {
        return GameRootDir() / "Binaries";
    }

    Common::Path Paths::GameConfigDir()
    {
        return GameRootDir() / "Config";
    }

    Common::Path Paths::GameCacheDir()
    {
        return GameRootDir() / "Cache";
    }

    Common::Path Paths::GameLogDir()
    {
        return GameCacheDir() / "Log";
    }

    Common::Path Paths::GamePluginDir()
    {
        return GameRootDir() / "Plugin";
    }

    Common::Path Paths::GamePluginAssetDir(const std::string& pluginName)
    {
        return GamePluginDir() / pluginName / "Asset";
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
