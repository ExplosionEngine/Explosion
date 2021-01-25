//
// Created by Administrator on 2021/1/9 0009.
//

#ifndef EXPLOSION_DRIVER_H
#define EXPLOSION_DRIVER_H

#include <memory>

#include <driver/context.h>

namespace Explosion {
    class Driver {
    public:
        virtual ~Driver() = 0;

    protected:
        Driver() = default;

    private:
    };
}

#endif //EXPLOSION_DRIVER_H
