//
// Created by johnk on 2023/8/7.
//

#pragma once

#include <RHI/RHIModule.h>
#include <RHI/Vulkan/Api.h>

namespace RHI::Vulkan {
    class RHI_VULKAN_API VulkanRHIModule final : public RHIModule {
    public:
        VulkanRHIModule();
        ~VulkanRHIModule() override;

        void OnLoad() override;
        void OnUnload() override;
        Instance* GetRHIInstance() override;
    };
}
