//
// Created by johnk on 27/12/2021.
//

#ifndef EXPLOSION_RHI_INSTANCE_H
#define EXPLOSION_RHI_INSTANCE_H

#include <Common/Utility.h>
#include <string>

namespace RHI {
    class PhysicalDevice;
    class LogicalDevice;
    class Surface;
    class ShaderCompiler;
    struct SurfaceCreateInfo;
    struct LogicalDeviceCreateInfo;

    struct InstanceCreateInfo {
        bool debugMode;
        size_t extensionNum;
        const char** extensions;
    };

    class Instance {
    public:
        static Instance* CreateByPlatform(const InstanceCreateInfo* info);

        NON_COPYABLE(Instance)
        virtual ~Instance();

        virtual ShaderCompiler* GetShaderCompiler() = 0;
        virtual uint32_t CountPhysicalDevices() = 0;
        virtual PhysicalDevice* GetPhysicalDevice(uint32_t idx) = 0;
        virtual LogicalDevice* CreateLogicalDevice(PhysicalDevice* physicalDevice, const LogicalDeviceCreateInfo* createInfo) = 0;
        virtual void DestroyLogicalDevice(LogicalDevice* logicalDevice) = 0;
        virtual Surface* CreateSurface(const SurfaceCreateInfo* createInfo) = 0;
        virtual void DestroySurface(Surface* surface) = 0;

    protected:
        explicit Instance(const InstanceCreateInfo* info);
    };
}

using RHICreateInstanceFunc = RHI::Instance*(*)(const RHI::InstanceCreateInfo*);

#endif //EXPLOSION_RHI_INSTANCE_H
