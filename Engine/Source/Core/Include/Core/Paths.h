//
// Created by johnk on 2023/7/31.
//

#pragma once

#include <filesystem>
#include <string>

#include <Core/Api.h>

namespace Core {
    class CORE_API Paths {
    public:
        Paths() = delete;

        static std::filesystem::path WorkingDir();
        static std::filesystem::path EngineRoot();
        static std::filesystem::path EngineRes();
        static std::filesystem::path EngineShader();
        static std::filesystem::path EngineAsset();
        static std::filesystem::path EngineBin();
        static std::filesystem::path EnginePlugin();
        static std::filesystem::path EnginePluginAsset(const std::string& pluginName);
        static std::filesystem::path ProjectFile();
        static void SetCurrentProjectFile(std::filesystem::path inFile);
        static std::filesystem::path ProjectRoot();
        static std::filesystem::path ProjectAsset();
        static std::filesystem::path ProjectBin();
        static std::filesystem::path ProjectPlugin();
        static std::filesystem::path ProjectPluginAsset(const std::string& pluginName);
        static std::filesystem::path EngineCMakeSourceDir();
        static std::filesystem::path EngineCMakeBinaryDir();

#if BUILD_TEST
        static std::filesystem::path EngineTest();
#endif

    private:
        static std::filesystem::path workdingDir;
        static std::filesystem::path currentProjectFile;
    };
}
