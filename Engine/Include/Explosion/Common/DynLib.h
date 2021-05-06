//
// Created by LiZhen on 2021/5/6.
//

#ifndef EXPLOSION_DYNLIB_H
#define EXPLOSION_DYNLIB_H

#include <string>

#ifdef _WIN32
    #include <Windows.h>
    #define LIB_HANDLE         HINSTANCE
    #define LIB_LOAD(lib, tag) LoadLibraryEx(lib, nullptr, 0)
    #define LIB_GET(lib, func) GetProcAddress(lib, func)
    #define LIB_UNLOAD(lib)    !FreeLibrary(lib)
#elif __APPLE__
    #include <dlfcn.h>
    #define LIB_HANDLE         void*
    #define LIB_LOAD(lib, tag) dlopen(lib, tag)
    #define LIB_GET(lib, func) dlsym(lib, func)
    #define LIB_UNLOAD(lib)    dlclose(lib)
#endif

namespace Explosion {
    class DynLib {
    public:
        DynLib(const std::string& n) : name(n) {}
        ~DynLib() {}

        void Load();

        void Unload();

        bool Valid();

        void* GetSymbol(const std::string& func) const;

    private:
        LIB_HANDLE handle;
        std::string name;
    };
}

#endif