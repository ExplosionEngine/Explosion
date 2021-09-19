//
// Created by Zach Lee on 2021/8/22.
//

#include <Engine/Render/Render.h>
#include <Common/Logger.h>
#include <Common/Exception.h>
#include <sstream>
#include <iostream>

namespace Explosion {

    Render::Render(const Descriptor& ci)
        : driver(RHI::DriverFactory::Singleton().CreateFromLib(ci.rhiName))
        , renderThread("renderThread")
    {
    }

    Render::~Render()
    {
        renderThread.ExitThread();
    }

    Render* Render::CreateRender(const Descriptor& ci)
    {
        auto render = new Render(ci);
        render->InitRHI(ci);
        render->InitRenderThread();
        return render;
    }

    void Render::DestroyRender(Render* render)
    {
        if (render != nullptr) {
            delete render;
        }
    }

    void Render::Tick(float time)
    {
        renderThread.Notify();
    }

    RHI::Driver* Render::GetDriver()
    {
        return driver.get();
    }

    void Render::RenderMain()
    {
        std::cout << "render" << std::endl;
    }

    void Render::InitRHI(const Descriptor& ci)
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

        mainCommandBuffer = driver->CreateCommandBuffer();
    }

    void Render::InitRenderThread()
    {
        renderThread.StartThread(&Render::RenderMain, this);
    }


}