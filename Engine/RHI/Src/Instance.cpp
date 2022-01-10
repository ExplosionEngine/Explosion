//
// Created by johnk on 9/1/2022.
//

#include <Common/DynamicLibrary.h>
#include <RHI/Instance.h>

namespace RHI {
    std::string GetPlatformRHILibName()
    {
#if PLATFORM_WINDOWS
        return "RHI-DirectX12";
#else
        return "RHI-Vulkan";
#endif
    }

    Instance* Instance::CreateInstanceByPlatform()
    {
        auto* dynamicLibrary = Common::DynamicLibraryManager::Singleton().FindOrLoad(GetPlatformRHILibName());
        if (dynamicLibrary == nullptr) {
            return nullptr;
        }
        RHICreateInstanceFunc symbol = static_cast<RHICreateInstanceFunc>(dynamicLibrary->GetSymbol("RHICreateInstance"));
        if (symbol == nullptr) {
            return nullptr;
        }
        return symbol();
    }

    Instance::~Instance() = default;

    Instance::Instance() = default;
}
