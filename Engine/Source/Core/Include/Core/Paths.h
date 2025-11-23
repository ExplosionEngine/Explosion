//
// Created by johnk on 2023/7/31.
//

#pragma once

#include <string>

#include <Core/Api.h>
#include <Common/FileSystem.h>

namespace Core {
    class CORE_API Paths {
    public:
        Paths() = delete;

        static void SetExecutableDir(const Common::Path& inPath);
        static void SetGameRoot(const Common::Path& inPath);

        static bool HasSetExecutableDir();
        static bool HasSetGameRoot();
        static Common::Path WorkingDir();
        static Common::Path ExecutablePath();
        static Common::Path EngineRootDir();
        static Common::Path EngineResDir();
        static Common::Path EngineShaderDir();
        static Common::Path EngineAssetDir();
        static Common::Path EngineBinDir();
        static Common::Path EngineConfigDir();
        static Common::Path EngineCacheDir();
        static Common::Path EngineLogDir();
        static Common::Path EnginePluginDir();
        static Common::Path EnginePluginDir(const std::string& pluginName);
        static Common::Path EnginePluginAssetDir(const std::string& pluginName);
        static Common::Path GameRootDir();
        static Common::Path GameAssetDir();
        static Common::Path GameBinDir();
        static Common::Path GameConfigDir();
        static Common::Path GameCacheDir();
        static Common::Path GameLogDir();
        static Common::Path GamePluginDir();
        static Common::Path GamePluginDir(const std::string& pluginName);
        static Common::Path GamePluginAssetDir(const std::string& pluginName);
#if BUILD_TEST
        static Common::Path EngineTestDir();
#endif

        static bool IsEnginePath(const Common::Path& inPath);
        static bool IsGamePath(const Common::Path& inPath);
        static bool IsEnginePluginPath(const Common::Path& inPath);
        static bool IsGamePluginPath(const Common::Path& inPath);
#if BUILD_TEST
        static bool IsEngineTestPath(const Common::Path& inPath);
#endif
        static Common::Path Translate(const Common::Path& inPath);
        static Common::Path TranslateAsset(const Common::Path& inPath);

    private:
        static Common::Path executablePath;
        static Common::Path workingDir;
        static Common::Path gameRoot;
    };
}
