//
// Created by johnk on 11/1/2022.
//

#ifndef EXPLOSION_RHI_VULKAN_INSTANCE_H
#define EXPLOSION_RHI_VULKAN_INSTANCE_H

#include <vulkan/vulkan.hpp>

#include <RHI/Instance.h>
#include <RHI/Vulkan/Api.h>

namespace RHI::Vulkan {
    class RHI_VULKAN_API VKInstance : public Instance {
    public:
        NON_COPYABLE(VKInstance)
        VKInstance();
        ~VKInstance() override;
        RHIType GetRHIType() override;

    private:
        void PrepareExtensions();
        void CreateVKInstance();
        void DestroyVKInstance();

        std::vector<const char*> vkEnabledExtensionNames;
        vk::Instance vkInstance;
    };
}

extern "C" {
    RHI_VULKAN_API RHI::Instance* RHICreateInstance();
}

#endif //EXPLOSION_RHI_VULKAN_INSTANCE_H
