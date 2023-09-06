//
// Created by johnk on 2023/8/7.
//

#pragma once

#include <Core/Module.h>
#include <RHI/RHIModule.h>
#include <RHI/Vulkan/Api.h>

namespace RHI::Vulkan {
    class RHI_VULKAN_API VulkanRHIModule : public RHIModule {
    public:
        VulkanRHIModule();
        ~VulkanRHIModule() override;

        Instance* GetRHIInstance() override;
    };
}
