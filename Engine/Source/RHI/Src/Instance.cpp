//
// Created by johnk on 9/1/2022.
//

#include <RHI/Instance.h>
#include <RHI/RHIModule.h>

namespace RHI {
    static RHIType GetPlatformRHIType()
    {
#if PLATFORM_WINDOWS
        return RHIType::directX12;
#else
        return RHIType::vulkan;
#endif
    }

    static std::string GetRHIModuleNameByType(const RHIType& type)
    {
        switch (type) {
            case RHIType::directX12:
                return "RHI-DirectX12";
            case RHIType::metal:
                return "RHI-Metal";
            case RHIType::dummy:
                return "RHI-Dummy";
            default:
                return "RHI-Vulkan";
        }
    }

    Instance* Instance::GetByPlatform()
    {
        return GetByType(GetPlatformRHIType());
    }

    Instance* Instance::GetByType(const RHIType& type)
    {
        auto* module = Core::ModuleManager::Get().FindOrLoadTyped<RHIModule>(GetRHIModuleNameByType(type));
        if (module == nullptr) {
            return nullptr;
        }
        return module->GetRHIInstance();
    }

    void Instance::UnloadByType(const RHIType& type)
    {
        Core::ModuleManager::Get().Unload(GetRHIModuleNameByType(type));
    }

    void Instance::UnloadAllInstances()
    {
        for (uint32_t i = static_cast<uint32_t>(RHIType::directX12); i < static_cast<uint32_t>(RHIType::max); i++) {
            RHIType rhiType = static_cast<RHIType>(i);
            Core::ModuleManager::Get().Unload(GetRHIModuleNameByType(rhiType));
        }
    }

    Instance::~Instance() = default;

    Instance::Instance() = default;
}
