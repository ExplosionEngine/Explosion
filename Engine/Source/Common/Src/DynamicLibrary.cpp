//
// Created by johnk on 28/12/2021.
//

#include <Common/DynamicLibrary.h>

namespace Common {
#if !PLATFORM_WINDOWS
    void RepairLibPrefix(std::string& name)
    {
        if (name.find("lib") != std::string::npos) {
            return;
        }
        name = "lib" + name;
    }
#endif

    void RepairExtension(std::string& name)
    {
#if PLATFORM_WINDOWS
        static const std::string ext = "dll";
#elif PLATFORM_MACOS
        static const std::string ext = "dylib";
#else
        static const std::string ext = "so";
#endif
        if (name.substr(name.find_last_of('.') + 1) == ext) {
            return;
        }
        name.append(".").append(ext);
    }

    std::string GetPlatformDynLibFullName(const std::string& simpleName)
    {
        std::string result = simpleName;
#if !PLATFORM_WINDOWS
        RepairLibPrefix(result);
#endif
        RepairExtension(result);
        return result;
    }
}

namespace Common {
    DynamicLibrary::DynamicLibrary(DYNAMIC_LIB_HANDLE h) : handle(h) {}

    DynamicLibrary::~DynamicLibrary() = default;

    void* DynamicLibrary::GetSymbol(const std::string &name)
    {
        return static_cast<void*>(DYNAMIC_LIB_GET_SYMBOL(handle, name.c_str()));
    }

    DYNAMIC_LIB_HANDLE DynamicLibrary::GetHandle()
    {
        return handle;
    }
}

namespace Common {
    DynamicLibraryManager& DynamicLibraryManager::Singleton()
    {
        static DynamicLibraryManager instance;
        return instance;
    }

    DynamicLibraryManager::DynamicLibraryManager() : libs() {}

    DynamicLibraryManager::~DynamicLibraryManager()
    {
        for (auto&& iter : libs) {
            DYNAMIC_LIB_UNLOAD(iter.second->GetHandle());
        }
    }

    DynamicLibrary* DynamicLibraryManager::FindOrLoad(const std::string& name)
    {
        auto fullName = GetPlatformDynLibFullName(name);
        auto iter = libs.find(fullName);
        if (iter == libs.end()) {
            DYNAMIC_LIB_HANDLE handle = DYNAMIC_LIB_LOAD(fullName.c_str(), RTLD_LOCAL | RTLD_LAZY);
            if (handle == nullptr) {
                return nullptr;
            }
            libs[fullName] = Common::MakeUnique<DynamicLibrary>(handle);
        }
        return libs[fullName].Get();
    }

    void DynamicLibraryManager::Unload(const std::string& name)
    {
        auto iter = libs.find(name);
        if (iter == libs.end()) {
            return;
        }
        DYNAMIC_LIB_UNLOAD(iter->second->GetHandle());
    }
}
