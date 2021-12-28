//
// Created by johnk on 28/12/2021.
//

#ifndef EXPLOSION_COMMON_DYNAMIC_LIBRARY_H
#define EXPLOSION_COMMON_DYNAMIC_LIBRARY_H

#include <string>
#include <utility>
#include <memory>
#include <unordered_map>

#include <Common/Utility.h>

#ifdef _WIN32
#include <Windows.h>
#define DYNAMIC_LIB_HANDLE               HINSTANCE
#define DYNAMIC_LIB_LOAD(lib, tag)       LoadLibraryEx(lib, nullptr, 0)
#define DYNAMIC_LIB_GET_SYMBOL(lib, sym) GetProcAddress(lib, sym)
#define DYNAMIC_LIB_UNLOAD(lib)          FreeLibrary(lib)
#else
#include <dlfcn.h>
#define DYNAMIC_LIB_HANDLE               void*
#define DYNAMIC_LIB_LOAD(lib, tag)       dlopen(lib, tag)
#define DYNAMIC_LIB_GET_SYMBOL(lib, sym) dlsym(lib, sym)
#define DYNAMIC_LIB_UNLOAD(lib)          dlclose(lib)
#endif

namespace Common {
    std::string GetPlatformDynLibFullName(const std::string& simpleName);

    class DynamicLibrary {
    public:
        NON_COPYABLE(DynamicLibrary)
        explicit DynamicLibrary(DYNAMIC_LIB_HANDLE h);
        ~DynamicLibrary();

        void* GetSymbol(const std::string& name);
        DYNAMIC_LIB_HANDLE GetHandle();

    private:
        DYNAMIC_LIB_HANDLE handle;
    };

    class DynamicLibraryManager {
    public:
        static DynamicLibraryManager& Singleton();

        NON_COPYABLE(DynamicLibraryManager)
        ~DynamicLibraryManager();

        DynamicLibrary* FindOrLoad(const std::string& name);
        void Unload(const std::string& name);

    private:
        DynamicLibraryManager();

        std::unordered_map<std::string, std::unique_ptr<DynamicLibrary>> libs;
    };
}

#endif //EXPLOSION_COMMON_DYNAMIC_LIBRARY_H
