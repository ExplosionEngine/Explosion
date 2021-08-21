#include <Common/DynLib.h>

namespace Explosion {
    void DynLib::Load()
    {
#ifndef _WIN32
        RepairLibPrefix();
#endif

#ifdef _WIN32
        RepairExtension("dll");
#elif __APPLE__
        RepairExtension("dylib");
#else
        RepairExtension("so");
#endif

        handle = LIB_LOAD(name.c_str(), RTLD_LOCAL | RTLD_LAZY);
    }

    void DynLib::Unload()
    {
        if (handle) {
            LIB_UNLOAD(handle);
        }
    }

    bool DynLib::Valid()
    {
        return handle != nullptr;
    }

    void* DynLib::GetSymbol(const std::string& func) const
    {
        return static_cast<void*>(LIB_GET(handle, func.c_str()));
    }

    void DynLib::RepairLibPrefix()
    {
        if (name.find("lib") != std::string::npos) {
            return;
        }
        name = "lib" + name;
    }

    void DynLib::RepairExtension(const std::string& extension)
    {
        if (name.substr(name.find_last_of('.') + 1) == extension) {
            return;
        }
        name += '.';
        name += extension;
    }
}