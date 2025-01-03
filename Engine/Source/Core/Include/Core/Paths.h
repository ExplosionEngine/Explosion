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
        static void SetCurrentProjectFile(const Common::Path& inFile);

        static bool HasProjectFile();
        static bool HasSetExecutableDir();
        static Common::Path WorkingDir();
        static Common::Path ExecutableDir();
        static Common::Path EngineRoot();
        static Common::Path EngineRes();
        static Common::Path EngineShader();
        static Common::Path EngineAsset();
        static Common::Path EngineBin();
        static Common::Path EnginePlugin();
        static Common::Path EnginePluginAsset(const std::string& pluginName);
        static Common::Path ProjectFile();
        static Common::Path ProjectRoot();
        static Common::Path ProjectAsset();
        static Common::Path ProjectBin();
        static Common::Path ProjectPlugin();
        static Common::Path ProjectPluginAsset(const std::string& pluginName);
        static Common::Path EngineCMakeSourceDir();
        static Common::Path EngineCMakeBinaryDir();

#if BUILD_TEST
        static Common::Path EngineTest();
#endif

    private:
        static Common::Path executableDir;
        static Common::Path workingDir;
        static Common::Path currentProjectFile;
    };
}
