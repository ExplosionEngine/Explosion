//
// Created by Zach Lee on 2021/5/6.
//

#ifndef EXPLOSION_DYNLIB_H
#define EXPLOSION_DYNLIB_H

#include <string>
#include <utility>

#if defined(_WIN32)
    #include <Windows.h>
    #define LIB_HANDLE         HINSTANCE
    #define LIB_LOAD(lib, tag) LoadLibraryEx(lib, nullptr, 0)
    #define LIB_GET(lib, func) GetProcAddress(lib, func)
    #define LIB_UNLOAD(lib)    FreeLibrary(lib)
#elif defined(__APPLE__) || defined(__linux__)
    #include <dlfcn.h>
    #define LIB_HANDLE         void*
    #define LIB_LOAD(lib, tag) dlopen(lib, tag)
    #define LIB_GET(lib, func) dlsym(lib, func)
    #define LIB_UNLOAD(lib)    dlclose(lib)
#endif

namespace Explosion {
    class DynLib {
    public:
        explicit DynLib(std::string n) : name(std::move(n)) {}
        ~DynLib() = default;
        DynLib(DynLib&) = delete;
        void operator=(DynLib&) = delete;

        void Load();
        void Unload();
        bool Valid();
        [[nodiscard]] void* GetSymbol(const std::string& func) const;

    private:
        void RepairLibPrefix();
        void RepairExtension(const std::string& extension);

        LIB_HANDLE handle {};
        std::string name;
    };
}

#endif