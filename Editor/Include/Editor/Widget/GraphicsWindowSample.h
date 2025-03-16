//
// Created by Kindem on 2025/3/16.
//

#pragma once

#include <QWidget>
#include <QBoxLayout>

#include <Editor/Widget/GraphicsWindow.h>

namespace Editor {
    class GraphicsWindowSample final : public GraphicsWindow {
        Q_OBJECT

    public:
        explicit GraphicsWindowSample(QWindow* inParent = nullptr);

    private:
        static constexpr uint8_t swapChainTextureNum = 2;
        static GraphicsWindowDesc GetGraphicsWindowDesc();

        Common::UniquePtr<RHI::Semaphore> imageReadySemaphore;
        Common::UniquePtr<RHI::Semaphore> renderFinishedSemaphore;
        Common::UniquePtr<RHI::Fence> frameFence;
        std::array<RHI::Texture*, 2> swapChainTextures;
        std::array<Common::UniquePtr<RHI::TextureView>, 2> swapChainTextureViews;
    };

    class GraphicsWindowSampleWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit GraphicsWindowSampleWidget(QWidget* inParent = nullptr);

    private:
        GraphicsWindowSample* graphicsWindow;
        QWidget* graphicsWindowContainer;
        QVBoxLayout* mainLayout;
    };
}
