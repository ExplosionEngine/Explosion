//
// Created by johnk on 2023/8/2.
//

#pragma once

#include <string>

#include <Common/DynamicLibrary.h>

#define IMPLEMENT_MODULE(moduleClass) \
    extern "C" Core::Module* GetModule() \
    { \
        static moduleClass instance; \
        return &instance; \
    } \

namespace Core {
    enum class ModuleType {
        engineModule,
        gameModule,
        enginePlugin,
        gamePlugin,
        max
    };

    class Module {
    public:
        virtual ~Module();
        virtual std::string GetName() const = 0;
        virtual ModuleType GetType() const = 0;
        virtual void OnLoad() = 0;
        virtual void OnUnload() = 0;

    protected:
        Module();
    };

    class ModuleRuntimeInfo {
        Module* instance;
        Common::DynamicLibrary* dynamicLib;
    };

    class ModuleManager {
    public:
        static ModuleManager& Get();
        ~ModuleManager();

    private:
        ModuleManager();
    };
}
