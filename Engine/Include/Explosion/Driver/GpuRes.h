//
// Created by John Kindem on 2021/5/2.
//

#ifndef EXPLOSION_GPURES_H
#define EXPLOSION_GPURES_H

namespace Explosion {
    class Driver;

    class GpuRes {
    public:
        explicit GpuRes(Driver& driver);
        virtual ~GpuRes();
        void Create();
        void Destroy();

    protected:
        virtual void OnCreate();
        virtual void OnDestroy();

        Driver& driver;
    };
}

#endif //EXPLOSION_GPURES_H
