//
// Created by johnk on 2023/8/2.
//

#include <Core/Module.h>
#include <Core/Paths.h>
#include <Common/String.h>

namespace Core {
    Module::Module() = default;

    Module::~Module() = default;

    void Module::OnLoad()
    {
    }

    void Module::OnUnload()
    {
    }

    ModuleRuntimeInfo::ModuleRuntimeInfo()
        : instance(nullptr)
    {
    }

    ModuleManager& ModuleManager::Get()
    {
        static ModuleManager instance;
        return instance;
    }

    ModuleManager::ModuleManager() = default;

    Module* ModuleManager::FindOrLoad(const std::string& moduleName)
    {
        if (const auto iter = loadedModules.find(moduleName);
            iter != loadedModules.end()) {
            return iter->second.instance;
        }

        if (!Load(moduleName)) {
            return nullptr;
        }
        return loadedModules.at(moduleName).instance;
    }

    Module* ModuleManager::Find(const std::string& moduleName)
    {
        const auto iter = loadedModules.find(moduleName);
        if (iter == loadedModules.end()) {
            return nullptr;
        }
        return iter->second.instance;
    }

    Module* ModuleManager::GetOrLoad(const std::string& moduleName)
    {
        auto* result = FindOrLoad(moduleName);
        Assert(result);
        return result;
    }

    Module* ModuleManager::Get(const std::string& moduleName)
    {
        auto* result = Find(moduleName);
        Assert(result);
        return result;
    }

    void ModuleManager::Register(const std::string& inName, Module& inStaticModule)
    {
        if (loadedModules.contains(inName)) {
            return;
        }

        ModuleRuntimeInfo moduleRuntimeInfo {};
        moduleRuntimeInfo.instance = &inStaticModule;
        moduleRuntimeInfo.instance->OnLoad();
        loadedModules.emplace(inName, std::move(moduleRuntimeInfo));
    }

    bool ModuleManager::Load(const std::string& moduleName)
    {
        if (loadedModules.contains(moduleName)) {
            return true;
        }

        const std::optional<std::string> modulePath = SearchModule(moduleName);
        if (!modulePath.has_value()) {
            return false;
        }

        Common::DynamicLibrary dynamicLib = Common::DynamicLibraryFinder::Find(modulePath.value());
        if (!dynamicLib.IsValid()) {
            return false;
        }
        dynamicLib.Load();

        const auto getModuleFunc = reinterpret_cast<GetModuleFunc>(dynamicLib.GetSymbol("GetModule"));
        if (getModuleFunc == nullptr) {
            return false;
        }

        ModuleRuntimeInfo moduleRuntimeInfo {};
        moduleRuntimeInfo.instance = getModuleFunc();
        moduleRuntimeInfo.dynamicLib = std::move(dynamicLib);
        moduleRuntimeInfo.instance->OnLoad();

        loadedModules.emplace(moduleName, std::move(moduleRuntimeInfo));
        return true;
    }

    void ModuleManager::Unload(const std::string& moduleName)
    {
        const auto iter = loadedModules.find(moduleName);
        if (iter == loadedModules.end()) {
            return;
        }
        iter->second.instance->OnUnload();
        loadedModules.erase(moduleName);
    }

    void ModuleManager::UnloadAll()
    {
        for (const auto& [moduleName, moduleRuntimeInfo] : loadedModules) {
            moduleRuntimeInfo.instance->OnUnload();
        }
        loadedModules.clear();
    }

    std::optional<std::string> ModuleManager::SearchModule(const std::string& moduleName)
    {
        std::vector<Common::Path> searchPaths;
        searchPaths.reserve(5);
        searchPaths.emplace_back(Paths::WorkingDir());
        if (Paths::HasSetExecutableDir()) {
            searchPaths.emplace_back(Paths::EngineBinDir());
            searchPaths.emplace_back(Paths::EnginePluginDir());
        }
        if (Paths::HasSetGameRoot()) {
            searchPaths.emplace_back(Paths::GameBinDir());
            searchPaths.emplace_back(Paths::GamePluginDir());
        }

        for (const auto& searchPath : searchPaths) {
            std::optional<std::string> result = std::nullopt;
            (void) searchPath.TraverseRecurse([&](const Common::Path& path) -> bool {
                if (path.IsDirectory()) {
                    return true;
                }

                const auto fileName = path.FileName();
                const auto extension = path.Extension(); // NOLINT

                if ((extension == ".dll" || extension == ".so" || extension == ".dylib")
                    && (fileName == std::format("{}{}", moduleName, extension) || fileName == std::format("lib{}{}", moduleName, extension))) {
                    result = path.String();
                    return false;
                }
                return true;
            });

            if (result.has_value()) {
                return result;
            }
        }
        return {};
    }
}
