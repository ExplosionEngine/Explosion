//
// Created by johnk on 9/1/2022.
//

#include <Common/DynamicLibrary.h>
#include <RHI/Instance.h>

namespace RHI {
    std::string GetPlatformRHILibName()
    {
#ifdef _WIN32
        return "RHI-DirectX12";
#else
        return "RHI-Vulkan";
#endif
    }

    Instance* Instance::CreateInstanceByPlatform(const InstanceCreateInfo* createInfo)
    {
        auto* dynamicLibrary = Common::DynamicLibraryManager::Singleton().FindOrLoad(GetPlatformRHILibName());
        if (dynamicLibrary == nullptr) {
            return nullptr;
        }
        RHICreateInstanceFunc symbol = static_cast<RHICreateInstanceFunc>(dynamicLibrary->GetSymbol("RHICreateInstance"));
        if (symbol == nullptr) {
            return nullptr;
        }
        return symbol(createInfo);
    }

    Instance::~Instance() = default;

    Instance::Instance(const InstanceCreateInfo& createInfo) {}
}
