//
// Created by johnk on 2023/8/2.
//

#pragma once

#include <string>
#include <optional>

#include <Common/DynamicLibrary.h>
#include <Core/Api.h>

#define IMPLEMENT_MODULE(apiName, moduleClass) \
    extern "C" apiName Core::Module* GetModule() \
    { \
        static moduleClass instance; \
        return &instance; \
    } \

namespace Core {
    class CORE_API Module {
    public:
        virtual ~Module();
        virtual void OnLoad();
        virtual void OnUnload();

    protected:
        Module();
    };

    using GetModuleFunc = Module*(*)();

    struct ModuleRuntimeInfo {
        Module* instance;
        Common::DynamicLibrary* dynamicLib;
    };

    class CORE_API ModuleManager {
    public:
        static ModuleManager& Get();
        ~ModuleManager();

        Module* FindOrLoad(const std::string& moduleName);

        template <typename T>
        T* FindOrLoadTyped(const std::string& moduleName)
        {
            return static_cast<T*>(FindOrLoad(moduleName));
        }

        void Unload(const std::string& moduleName);
        void UnloadAll();

    private:
        static std::optional<std::string> SearchModule(const std::string& moduleName);

        ModuleManager();
        std::unordered_map<std::string, ModuleRuntimeInfo> loadedModules;
    };
}
