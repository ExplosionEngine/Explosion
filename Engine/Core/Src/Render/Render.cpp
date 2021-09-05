//
// Created by LiZhen on 2021/8/22.
//

#include <Engine/Render/Render.h>
#include <Common/Logger.h>
#include <Common/Exception.h>
#include <sstream>
#include <iostream>

namespace Explosion {

    std::unique_ptr<Render> g_render;

    Render::Render(const RenderCreateInfo& ci)
        : driver(RHI::DriverFactory::Singleton().CreateFromLib(ci.rhiName))
        , renderThread("renderThread")
    {
    }

    Render::~Render()
    {
        renderThread.ExitThread();
    }

    Render* Render::CreateRender(const RenderCreateInfo& ci)
    {
        if (!g_render) {
            g_render.reset(new Render(ci));
            g_render->InitRHI(ci);
            g_render->InitRenderThread();
        }
        return g_render.get();
    }

    void Render::DestroyRender()
    {
        g_render.reset();
    }

    Render* Render::GetRender()
    {
        EXPLOSION_ASSERT(g_render.get() != nullptr, "CreateRender should be called first");
        return g_render.get();
    }

    void Render::Tick(float time)
    {
        renderThread.Notify();
    }

    void Render::RenderMain()
    {
        std::cout << "render" << std::endl;
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

        mainCommandBuffer = driver->CreateCommandBuffer();
    }

    void Render::InitRenderThread()
    {
        renderThread.StartThread(&Render::RenderMain, this);
    }


}