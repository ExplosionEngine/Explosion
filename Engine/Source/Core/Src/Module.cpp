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

    ModuleManager& ModuleManager::Get()
    {
        static ModuleManager instance;
        return instance;
    }

    ModuleManager::ModuleManager() = default;

    ModuleManager::~ModuleManager()
    {
        UnloadAll();
    }

    Module* ModuleManager::FindOrLoad(const std::string& moduleName)
    {
        auto iter = loadedModules.find(moduleName);
        if (iter != loadedModules.end()) {
            return iter->second.instance;
        }

        std::optional<std::string> modulePath = SearchModule(moduleName);
        if (!modulePath.has_value()) {
            return nullptr;
        }
        Common::DynamicLibrary* dynamicLib = Common::DynamicLibraryManager::Get().FindOrLoad(modulePath.value());
        if (dynamicLib == nullptr) {
            return nullptr;
        }
        GetModuleFunc getModuleFunc = reinterpret_cast<GetModuleFunc>(dynamicLib->GetSymbol("GetModule"));
        if (getModuleFunc == nullptr) {
            return nullptr;
        }

        ModuleRuntimeInfo moduleRuntimeInfo;
        moduleRuntimeInfo.instance = getModuleFunc();
        moduleRuntimeInfo.dynamicLib = dynamicLib;
        moduleRuntimeInfo.instance->OnLoad();

        loadedModules.emplace(moduleName, moduleRuntimeInfo);
        return loadedModules[moduleName].instance;
    }

    Module* ModuleManager::Find(const std::string& moduleName)
    {
        auto iter = loadedModules.find(moduleName);
        if (iter == loadedModules.end()) {
            return nullptr;
        }
        return iter->second.instance;
    }

    void ModuleManager::Unload(const std::string& moduleName)
    {
        auto iter = loadedModules.find(moduleName);
        if (iter == loadedModules.end()) {
            return;
        }
        iter->second.instance->OnUnload();
        loadedModules.erase(moduleName);
    }

    void ModuleManager::UnloadAll()
    {
        for (auto& loadedModule : loadedModules) {
            loadedModule.second.instance->OnLoad();
        }
        loadedModules.clear();
    }

    std::optional<std::string> ModuleManager::SearchModule(const std::string& moduleName)
    {
        const std::vector<std::filesystem::path> searchPaths = {
            Paths::EngineBinariesPath(),
            Paths::ProjectBinariesPath(),
            Paths::EnginePluginPath(),
            Paths::ProjectPluginPath()
        };
        const std::filesystem::path workingDir = Paths::WorkingDir();

        for (const auto& searchPath : searchPaths) {
            try {
                for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(searchPath)) {
                    if (entry.is_directory()) {
                        continue;
                    }

                    const auto& path = entry.path();
                    auto fileName = path.filename().string();
                    auto extension = path.extension().string();

                    if ((extension == ".dll" || extension == ".so" || extension == ".dylib")
                        && (fileName == fmt::format("{}{}", moduleName, extension) || fileName == fmt::format("lib{}{}", moduleName, extension)))
                    {
                        return path.string();
                    }
                }
            } catch (const std::exception& e) {
                continue;
            }
        }
        return {};
    }
}
