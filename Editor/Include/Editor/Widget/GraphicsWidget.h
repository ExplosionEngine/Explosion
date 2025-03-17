//
// Created by Kindem on 2025/3/16.
//

#pragma once

#include <QWidget>
#include <QTimer>

#include <Common/Memory.h>
#include <RHI/RHI.h>

namespace Editor {
    struct GraphicsWidgetDesc {
        uint8_t textureNum;
        std::vector<RHI::PixelFormat> formatQualifiers;
        RHI::PresentMode presentMode;
    };

    class GraphicsWidget : public QWidget {
        Q_OBJECT

    public:
        explicit GraphicsWidget(const GraphicsWidgetDesc& inDesc, QWidget* inParent = nullptr);
        ~GraphicsWidget() override;

        uint8_t GetTextureNum() const;
        RHI::PixelFormat GetPixelFormat() const;
        RHI::PresentMode GetPresentMode() const;
        RHI::Device& GetDevice() const;
        RHI::Surface& GetSurface() const;
        RHI::SwapChain& GetSwapChain() const;

    protected:
        QPaintEngine* paintEngine() const override;
        void resizeEvent(QResizeEvent* event) override;

        virtual void OnDrawFrame();
        void WaitDeviceIdle() const;
        double GetLastTimeSeconds() const;
        double GetCurrentTimeSeconds() const;
        float GetDeltaTimeSeconds() const;

        uint8_t textureNum;
        RHI::PixelFormat pixelFormat;
        RHI::PresentMode presentMode;
        RHI::Device* device;
        Common::UniquePtr<RHI::Surface> surface;
        Common::UniquePtr<RHI::SwapChain> swapChain;
        QTimer* timer;
        double lastTimeSeconds;
        double currentTimeSeconds;
        float deltaTimeSeconds;
    };
}
