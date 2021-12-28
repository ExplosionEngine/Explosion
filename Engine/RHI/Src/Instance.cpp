//
// Created by johnk on 27/12/2021.
//

#include <RHI/Instance.h>
#include <Common/DynamicLibrary.h>

using namespace Common;

namespace {
    std::string GetPlatformRHILibName()
    {
#ifdef _WIN32
        return "RHI-DirectX12";
#else
        return "RHI-Vulkan";
#endif
    }
}

namespace RHI {
    Instance* Instance::CreateByPlatform()
    {
        DynamicLibrary* rhiLib = DynamicLibraryManager::Singleton().FindOrLoad(GetPlatformRHILibName());
        if (rhiLib == nullptr)
        {
            return nullptr;
        }
        RHICreateInstanceFunc symbol = static_cast<RHICreateInstanceFunc>(rhiLib->GetSymbol("RHICreateInstance"));
        if (symbol == nullptr)
        {
            return nullptr;
        }
        return symbol();
    }

    Instance::Instance() = default;
}
