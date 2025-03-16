//
// Created by Kindem on 2025/3/16.
//

#include <Editor/Widget/GraphicsWindow.h>
#include <Editor/Widget/moc_GraphicsWindow.cpp>

namespace Editor {
    GraphicsWindow::GraphicsWindow(const GraphicsWindowDesc& inDesc, QWindow* inParent)
        : QWindow(inParent)
        , textureNum(inDesc.textureNum)
    {
        const Render::RenderModule& renderModule = Core::ModuleManager::Get().GetTyped<Render::RenderModule>("Render");
        device = renderModule.GetDevice();
        Assert(device != nullptr);

        surface = device->CreateSurface(
            RHI::SurfaceCreateInfo()
                .SetWindow(reinterpret_cast<void*>(winId()))); // NOLINT

        pixelFormat = RHI::PixelFormat::max;
        for (const auto format : inDesc.formatQualifiers) {
            if (device->CheckSwapChainFormatSupport(surface.Get(), format)) {
                pixelFormat = format;
                break;
            }
        }

        swapChain = device->CreateSwapChain(
            RHI::SwapChainCreateInfo()
                .SetPresentQueue(device->GetQueue(RHI::QueueType::graphics, 0))
                .SetSurface(surface.Get())
                .SetTextureNum(inDesc.textureNum)
                .SetFormat(pixelFormat)
                .SetWidth(width())
                .SetHeight(height())
                .SetPresentMode(inDesc.presentMode));
    }

    GraphicsWindow::~GraphicsWindow()
    {
        const Common::UniquePtr<RHI::Fence> fence = device->CreateFence(false);
        device->GetQueue(RHI::QueueType::graphics, 0)->Flush(fence.Get());
        fence->Wait();
    }

    uint8_t GraphicsWindow::GetTextureNum() const
    {
        return textureNum;
    }

    RHI::PixelFormat GraphicsWindow::GetPixelFormat() const
    {
        return pixelFormat;
    }

    RHI::Device& GraphicsWindow::GetDevice() const
    {
        return *device;
    }

    RHI::Surface& GraphicsWindow::GetSurface() const
    {
        return *surface;
    }

    RHI::SwapChain& GraphicsWindow::GetSwapChain() const
    {
        return *swapChain;
    }
} // namespace Editor
