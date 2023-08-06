//
// Created by johnk on 2023/8/2.
//

#pragma once

#include <string>
#include <optional>

#include <Common/DynamicLibrary.h>
#include <Core/Api.h>

#define IMPLEMENT_MODULE(moduleClass) \
    extern "C" Core::Module* GetModule() \
    { \
        static moduleClass instance; \
        return &instance; \
    } \

namespace Core {
    class CORE_API Module {
    public:
        virtual ~Module();
        virtual void OnLoad() = 0;
        virtual void OnUnload() = 0;

    protected:
        Module();
    };

    using GetModuleFunc = Module*(*)();

    struct ModuleRuntimeInfo {
        Module* instance;
        Common::DynamicLibrary* dynamicLib;
    };

    class ModuleManager {
    public:
        static ModuleManager& Get();
        ~ModuleManager();

        Module* FindOrLoad(const std::string& moduleName);
        void Unload(const std::string& moduleName);
        void UnloadAll();

    private:
        static std::optional<std::string> SearchModule(const std::string& moduleName);

        ModuleManager();
        std::unordered_map<std::string, ModuleRuntimeInfo> loadedModules;
    };
}
