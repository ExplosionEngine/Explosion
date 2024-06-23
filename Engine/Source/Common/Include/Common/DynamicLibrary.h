//
// Created by johnk on 28/12/2021.
//

#pragma once

#include <string>
#include <unordered_map>

#include <Common/Utility.h>
#include <Common/Memory.h>

#if PLATFORM_WINDOWS
#define NOMINMAX
#include <windows.h>
#define DynamicLibHandle               HINSTANCE
#define DynamicLibLoad(lib, tag)       LoadLibraryEx(lib, nullptr, 0)
#define DynamicLibGetSymbol(lib, sym)  GetProcAddress(lib, sym)
#define DynamicLibUnload(lib)          FreeLibrary(lib)
#else
#include <dlfcn.h>
#define DynamicLibHandle               void*
#define DynamicLibLoad(lib, tag)       dlopen(lib, tag)
#define DynamicLibGetSymbol(lib, sym)  dlsym(lib, sym)
#define DynamicLibUnload(lib)          dlclose(lib)
#endif

namespace Common {
    class DynamicLibrary {
    public:
        NonCopyable(DynamicLibrary)
        explicit DynamicLibrary(std::string inFullPath);
        ~DynamicLibrary();

        void Load();
        void Unload();
        void* GetSymbol(const std::string& name) const;
        DynamicLibHandle GetHandle() const;

    private:
        bool active;
        std::string fullPath;
        DynamicLibHandle handle;
    };

    class DynamicLibraryFinder {
    public:
        static UniqueRef<DynamicLibrary> Find(const std::string& simpleName, const std::string& searchDirectory = "");

    private:
        static std::string GetPlatformDynLibFullPath(const std::string& simpleName, const std::string& searchDirectory);
        static void RepairLibPrefix(std::string& name);
        static void RepairExtension(std::string& name);
    };
}
