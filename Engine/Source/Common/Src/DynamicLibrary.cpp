//
// Created by johnk on 28/12/2021.
//

#include <Common/DynamicLibrary.h>
#include <Common/Debug.h>

namespace Common {
    DynamicLibrary::DynamicLibrary(std::string inFullPath)
        : active(false)
        , fullPath(std::move(inFullPath))
    {
    }

    DynamicLibrary::~DynamicLibrary()
    {
        if (active) {
            Unload();
        }
    }

    void DynamicLibrary::Load()
    {
        Assert(!active);
        handle = DynamicLibLoad(fullPath.c_str(), RTLD_LOCAL | RTLD_LAZY);
        Assert(handle != nullptr);
        active = true;
    }

    void DynamicLibrary::Unload()
    {
        Assert(active);
        DynamicLibUnload(handle);
        active = false;
    }

    void* DynamicLibrary::GetSymbol(const std::string& name)
    {
        Assert(active);
        return static_cast<void*>(DynamicLibGetSymbol(handle, name.c_str()));
    }

    DynamicLibHandle DynamicLibrary::GetHandle()
    {
        return handle;
    }

    Common::UniqueRef<DynamicLibrary> DynamicLibraryFinder::Find(const std::string& simpleName, const std::string& searchDirectory)
    {
        return Common::MakeUnique<DynamicLibrary>(GetPlatformDynLibFullPath(simpleName, searchDirectory));
    }

    std::string DynamicLibraryFinder::GetPlatformDynLibFullPath(const std::string& simpleName, const std::string& searchDirectory)
    {
        std::string result = simpleName;
        RepairLibPrefix(result);
        RepairExtension(result);

        if (searchDirectory.empty()) {
            return result;
        }
        return searchDirectory + "/" + result;
    }

    void DynamicLibraryFinder::RepairLibPrefix(std::string& name)
    {
#if !PLATFORM_WINDOWS
        if (name.find("lib") != std::string::npos) {
            return;
        }
        name = "lib" + name;
#endif
    }

    void DynamicLibraryFinder::RepairExtension(std::string& name)
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
}
