//
// Created by Kindem on 2025/3/16.
//

#include <Editor/Widget/GraphicsWidget.h>
#include <Editor/Widget/moc_GraphicsWidget.cpp> // NOLINT

namespace Editor {
    GraphicsWidget::GraphicsWidget(QWidget* inParent)
        : QWidget(inParent)
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
    }

    GraphicsWidget::~GraphicsWidget() = default;

    RHI::Device& GraphicsWidget::GetDevice() const
    {
        return *device;
    }

    RHI::Surface& GraphicsWidget::GetSurface() const
    {
        return *surface;
    }

    QPaintEngine* GraphicsWidget::paintEngine() const
    {
        return nullptr;
    }

    void GraphicsWidget::WaitDeviceIdle() const
    {
        const Common::UniquePtr<RHI::Fence> fence = device->CreateFence(false);
        device->GetQueue(RHI::QueueType::graphics, 0)->Flush(fence.Get());
        fence->Wait();
    }
} // namespace Editor
