//
// Created by Kindem on 2025/3/16.
//

#pragma once

#include <QWindow>

#include <Common/Memory.h>
#include <RHI/RHI.h>

namespace Editor {
    struct GraphicsWindowDesc {
        uint8_t textureNum;
        std::vector<RHI::PixelFormat> formatQualifiers;
        RHI::PresentMode presentMode;
    };

    class GraphicsWindow : public QWindow {
        Q_OBJECT

    public:
        explicit GraphicsWindow(const GraphicsWindowDesc& inDesc, QWindow* inParent = nullptr);
        ~GraphicsWindow() override;

        uint8_t GetTextureNum() const;
        RHI::PixelFormat GetPixelFormat() const;
        RHI::Device& GetDevice() const;
        RHI::Surface& GetSurface() const;
        RHI::SwapChain& GetSwapChain() const;

        // TODO auto resize

    private:
        uint8_t textureNum;
        RHI::PixelFormat pixelFormat;
        RHI::Device* device;
        Common::UniquePtr<RHI::Surface> surface;
        Common::UniquePtr<RHI::SwapChain> swapChain;
    };
}
