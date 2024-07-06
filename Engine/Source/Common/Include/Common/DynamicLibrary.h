//
// Created by johnk on 28/12/2021.
//

#pragma once

#include <string>

#include <Common/Memory.h>

#if PLATFORM_WINDOWS
#include <windows.h>
#define DynamicLibHandle HINSTANCE
#else
#define DynamicLibHandle void*
#endif

namespace Common {
    class DynamicLibrary {
    public:
        DynamicLibrary();
        explicit DynamicLibrary(std::string inFullPath);
        ~DynamicLibrary();

        DynamicLibrary(DynamicLibrary&& inOther) noexcept;
        DynamicLibrary& operator=(DynamicLibrary&& inOther) noexcept;

        bool IsValid() const;
        bool IsLoaded() const;
        void Load();
        void Unload();
        void* GetSymbol(const std::string& name) const;
        DynamicLibHandle GetHandle() const;

    private:
        bool loaded;
        std::string fullPath;
        DynamicLibHandle handle;
    };

    class DynamicLibraryFinder {
    public:
        static DynamicLibrary Find(const std::string& simpleName, const std::string& searchDirectory = "");

    private:
        static std::string GetPlatformDynLibFullPath(const std::string& simpleName, const std::string& searchDirectory);
        static void RepairLibPrefix(std::string& name);
        static void RepairExtension(std::string& name);
    };
}
