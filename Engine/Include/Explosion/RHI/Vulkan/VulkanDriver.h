//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_VULKANDRIVER_H
#define EXPLOSION_VULKANDRIVER_H

#include <memory>

#include <Explosion/RHI/Vulkan/VulkanDevice.h>

namespace Explosion::RHI {
    class VulkanDriver {
    public:
        VulkanDriver();
        ~VulkanDriver();
        VulkanDevice* GetDevice();

        template <typename Type, typename... Args>
        Type* CreateGpuRes(const Args&... args)
        {
            return new Type(*this, args...);
        }

        template <typename Type>
        void DestroyGpuRes(Type* res)
        {
            delete res;
        }

    private:
        std::unique_ptr<VulkanDevice> device;
    };
}

#endif //EXPLOSION_VULKANDRIVER_H
