//
// Created by johnk on 2023/8/2.
//

#pragma once

#include <string>
#include <optional>
#include <unordered_map>

#include <Common/DynamicLibrary.h>
#include <Common/Debug.h>
#include <Core/Api.h>

// if your dynamic library always given interface by XXX_API nor module,
// just declare a min module as placeholder to compatible with module manager
#define DECLARE_MIN_MODULE(apiName, moduleClass, type) \
    class apiName moduleClass final : public Core::Module { \
        virtual Core::ModuleType Type() const { return type; } \
    }; \

#define IMPLEMENT_DYNAMIC_MODULE(apiName, moduleClass) \
    extern "C" apiName Core::Module* GetModule() \
    { \
        static moduleClass instance; \
        return &instance; \
    } \

#define IMPLEMENT_STATIC_MODULE(id, name, moduleClass) \
    int _register_##id = []() -> int { \
        static moduleClass instance; \
        Core::ModuleManager::Get().Register(name, instance); \
        return 0; \
    }(); \

namespace Core {
    enum class ModuleType : uint8_t {
        mStatic,
        mDynamic,
        max
    };

    class CORE_API Module {
    public:
        virtual ~Module();
        virtual void OnLoad();
        virtual void OnUnload();
        virtual ModuleType Type() const = 0;

    protected:
        Module();
    };

    template <typename T>
    concept ModuleDerived = std::is_base_of_v<Module, T>;

    using GetModuleFunc = Module*(*)();

    struct ModuleRuntimeInfo {
        NonCopyable(ModuleRuntimeInfo)
        DefaultMovable(ModuleRuntimeInfo)
        ModuleRuntimeInfo();

        Module* instance;
        Common::DynamicLibrary dynamicLib;
    };

    class CORE_API ModuleManager {
    public:
        static ModuleManager& Get();

        NonCopyable(ModuleManager)

        template <ModuleDerived T> T* FindOrLoadTyped(const std::string& moduleName);
        template <ModuleDerived T> T* FindTyped(const std::string& moduleName);
        template <ModuleDerived T> T& GetOrLoadTyped(const std::string& moduleName);
        template <ModuleDerived T> T& GetTyped(const std::string& moduleName);
        Module* FindOrLoad(const std::string& moduleName);
        Module* Find(const std::string& moduleName);
        Module* GetOrLoad(const std::string& moduleName);
        Module* Get(const std::string& moduleName);
        bool Load(const std::string& moduleName);
        void Register(const std::string& inName, Module& inStaticModule);

        void Unload(const std::string& moduleName);
        void UnloadAll();

    private:
        static std::optional<std::string> SearchModule(const std::string& moduleName);

        ModuleManager();
        std::unordered_map<std::string, ModuleRuntimeInfo> loadedModules;
    };
}

namespace Core {
    template <ModuleDerived T>
    T* ModuleManager::FindOrLoadTyped(const std::string& moduleName)
    {
        return static_cast<T*>(FindOrLoad(moduleName));
    }

    template <ModuleDerived T>
    T* ModuleManager::FindTyped(const std::string& moduleName)
    {
        return static_cast<T*>(Find(moduleName));
    }

    template <ModuleDerived T>
    T& ModuleManager::GetOrLoadTyped(const std::string& moduleName)
    {
        auto* result = FindOrLoadTyped<T>(moduleName);
        Assert(result);
        return *result;
    }

    template <ModuleDerived T>
    T& ModuleManager::GetTyped(const std::string& moduleName)
    {
        auto* result = FindTyped<T>(moduleName);
        Assert(result);
        return *result;
    }
}
