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
        const std::vector searchPaths = {
            Paths::EngineBin(),
            Paths::ProjectBin(),
            Paths::EnginePlugin(),
            Paths::ProjectPlugin()
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
