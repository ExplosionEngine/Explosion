//
// Created by johnk on 9/1/2022.
//

#include <Common/DynamicLibrary.h>
#include <RHI/Instance.h>

namespace RHI {
    static RHIType GetPlatformRHIType()
    {
#if PLATFORM_WINDOWS
        return RHIType::DIRECTX_12;
#else
        return RHIType::VULKAN;
#endif
    }

    static std::string GetRHILibNameByType(const RHIType& type)
    {
        switch (type) {
            case RHIType::DIRECTX_12:
                return "RHI-DirectX12";
            case RHIType::METAL:
                return "RHI-Metal";
            default:
                return "RHI-Vulkan";
        }
    }

    Instance* Instance::CreateByPlatform()
    {
        return CreateByType(GetPlatformRHIType());
    }

    Instance* Instance::CreateByType(const RHIType& type)
    {
        auto* dynamicLibrary = Common::DynamicLibraryManager::Singleton().FindOrLoad(GetRHILibNameByType(type));
        if (dynamicLibrary == nullptr) {
            return nullptr;
        }
        RHIGetInstanceFunc symbol = reinterpret_cast<RHIGetInstanceFunc>(dynamicLibrary->GetSymbol("RHIGetInstance"));
        if (symbol == nullptr) {
            return nullptr;
        }
        return symbol();
    }

    Instance::~Instance() = default;

    Instance::Instance() = default;
}
