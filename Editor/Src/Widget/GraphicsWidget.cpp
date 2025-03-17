//
// Created by Kindem on 2025/3/16.
//

#include <Common/Time.h>
#include <Editor/Widget/GraphicsWidget.h>
#include <Editor/Widget/moc_GraphicsWidget.cpp> // NOLINT

namespace Editor {
    GraphicsWidget::GraphicsWidget(const GraphicsWidgetDesc& inDesc, QWidget* inParent)
        : QWidget(inParent)
        , textureNum(inDesc.textureNum)
        , presentMode(inDesc.presentMode)
        , lastTimeSeconds(Common::TimePoint::Now().ToSeconds())
        , currentTimeSeconds(Common::TimePoint::Now().ToSeconds())
        , deltaTimeSeconds(0.0f)
    {
        setAttribute(Qt::WA_NativeWindow);
        setAttribute(Qt::WA_PaintOnScreen);
        setAttribute(Qt::WA_NoSystemBackground);

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

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &GraphicsWidget::OnDrawFrame);
        timer->start(0);
    }

    GraphicsWidget::~GraphicsWidget()
    {
        disconnect(timer, &QTimer::timeout, this, &GraphicsWidget::OnDrawFrame);
        timer->stop();

        WaitDeviceIdle();
    }

    uint8_t GraphicsWidget::GetTextureNum() const
    {
        return textureNum;
    }

    RHI::PixelFormat GraphicsWidget::GetPixelFormat() const
    {
        return pixelFormat;
    }

    RHI::PresentMode GraphicsWidget::GetPresentMode() const
    {
        return presentMode;
    }

    RHI::Device& GraphicsWidget::GetDevice() const
    {
        return *device;
    }

    RHI::Surface& GraphicsWidget::GetSurface() const
    {
        return *surface;
    }

    RHI::SwapChain& GraphicsWidget::GetSwapChain() const
    {
        return *swapChain;
    }

    QPaintEngine* GraphicsWidget::paintEngine() const
    {
        return nullptr;
    }

    void GraphicsWidget::resizeEvent(QResizeEvent* event)
    {
        WaitDeviceIdle();

        const auto newSize = event->size();

        swapChain.Reset();
        swapChain = device->CreateSwapChain(
            RHI::SwapChainCreateInfo()
                .SetPresentQueue(device->GetQueue(RHI::QueueType::graphics, 0))
                .SetSurface(surface.Get())
                .SetTextureNum(textureNum)
                .SetFormat(pixelFormat)
                .SetWidth(newSize.width())
                .SetHeight(newSize.height())
                .SetPresentMode(presentMode));
    }

    void GraphicsWidget::OnDrawFrame()
    {
        currentTimeSeconds = Common::TimePoint::Now().ToSeconds();
        deltaTimeSeconds = currentTimeSeconds - lastTimeSeconds;
        lastTimeSeconds = currentTimeSeconds;
    }

    void GraphicsWidget::WaitDeviceIdle() const
    {
        const Common::UniquePtr<RHI::Fence> fence = device->CreateFence(false);
        device->GetQueue(RHI::QueueType::graphics, 0)->Flush(fence.Get());
        fence->Wait();
    }

    double GraphicsWidget::GetLastTimeSeconds() const
    {
        return currentTimeSeconds;
    }

    double GraphicsWidget::GetCurrentTimeSeconds() const
    {
        return currentTimeSeconds;
    }

    float GraphicsWidget::GetDeltaTimeSeconds() const
    {
        return deltaTimeSeconds;
    }
} // namespace Editor
