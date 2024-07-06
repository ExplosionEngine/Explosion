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

        const std::optional<std::string> modulePath = SearchModule(moduleName);
        if (!modulePath.has_value()) {
            return nullptr;
        }

        Common::DynamicLibrary dynamicLib = Common::DynamicLibraryFinder::Find(modulePath.value());
        if (!dynamicLib.IsValid()) {
            return nullptr;
        }
        dynamicLib.Load();

        const auto getModuleFunc = reinterpret_cast<GetModuleFunc>(dynamicLib.GetSymbol("GetModule"));
        if (getModuleFunc == nullptr) {
            return nullptr;
        }

        ModuleRuntimeInfo moduleRuntimeInfo;
        moduleRuntimeInfo.instance = getModuleFunc();
        moduleRuntimeInfo.dynamicLib = std::move(dynamicLib);
        moduleRuntimeInfo.instance->OnLoad();

        loadedModules.emplace(moduleName, std::move(moduleRuntimeInfo));
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
        const std::vector searchPaths = {
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

                    if (auto extension = path.extension().string();
                        (extension == ".dll" || extension == ".so" || extension == ".dylib")
                        && (fileName == fmt::format("{}{}", moduleName, extension) || fileName == fmt::format("lib{}{}", moduleName, extension)))
                    {
                        return path.string();
                    }
                }
            } catch (const std::exception&) { // NOLINT
                // TODO maybe output log here
            }
        }
        return {};
    }
}
