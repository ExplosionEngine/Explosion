//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_DRIVER_H
#define EXPLOSION_DRIVER_H

#include <memory>

#include <Explosion/RHI/Device.h>

namespace Explosion::RHI {
    class Driver {
    public:
        Driver();
        ~Driver();
        Device* GetDevice();

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
        std::unique_ptr<Device> device;
    };
}

#endif //EXPLOSION_DRIVER_H
