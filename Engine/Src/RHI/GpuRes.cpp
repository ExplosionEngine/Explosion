//
// Created by John Kindem on 2021/5/2.
//

#include <Explosion/RHI/GpuRes.h>

namespace Explosion {
    GpuRes::GpuRes(Driver& driver) : driver(driver) {}

    GpuRes::~GpuRes() = default;

    void GpuRes::Create()
    {
        OnCreate();
    }

    void GpuRes::Destroy()
    {
        OnDestroy();
    }

    void GpuRes::OnCreate() {}

    void GpuRes::OnDestroy() {}
}
