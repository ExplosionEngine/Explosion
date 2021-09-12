//
// Created by LiZhen on 2021/9/12.
//

#ifndef EXPLOSION_DEVICECHILD_H
#define EXPLOSION_DEVICECHILD_H

namespace Explosion {

    class DeviceChild {
    public:
        DeviceChild() = default;
        ~DeviceChild() = default;

        enum class SubmitOption : uint8_t {
            STATIC  = 0x00,
            DYNAMIC = 0x01
        };

        void Submit();
    };
}

#endif // EXPLOSION_DEVICECHILD_H
