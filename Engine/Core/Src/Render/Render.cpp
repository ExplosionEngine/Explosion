//
// Created by LiZhen on 2021/8/22.
//

#include <Engine/Render/Render.h>
#include <Common/Logger.h>
#include <sstream>

namespace Explosion {

    Render::Render(const RenderCreateInfo& ci)
        : driver(RHI::DriverFactory::Singleton().CreateFromLib(ci.rhiName))
    {
        InitRHI(ci);
    }

    void Render::InitRHI(const RenderCreateInfo& ci)
    {
        const auto& devInfo = driver->GetDeviceInfo();

        std::stringstream ss;
        ss << "\n********************** RHI Init **********************\n"
        << "RHI name    : " << ci.rhiName << "\n"
        << "Vender      : " << devInfo.vender << "\n"
        << "Device Name : " << devInfo.deviceName << "\n"
        << "Device Type : " << static_cast<int>(devInfo.type) << "(0: INTERGRATED, 1: DISCRETE, 2: OTHER)\n"
        "********************** RHI Init **********************";
        Logger::Info(ss.str());
    }

    void Render::InitRenderThread()
    {

    }


}