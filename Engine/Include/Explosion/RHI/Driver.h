//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_DRIVER_H
#define EXPLOSION_DRIVER_H

#include <memory>

#include <Explosion/RHI/Device.h>

namespace Explosion {
    class Driver {
    public:
        Driver();
        ~Driver();
        Device* GetDevice();

        template <typename Type, typename... Args>
        Type* CreateGpuRes(const Args&... args)
        {
            Type* gpuRes = new Type(*this, args...);
            gpuRes->Create();
            return gpuRes;
        }

        template <typename Type>
        void DestroyGpuRes(Type* res)
        {
            res->Destroy();
            delete res;
        }

    private:
        std::unique_ptr<Device> device;
    };
}

#endif //EXPLOSION_DRIVER_Hw
