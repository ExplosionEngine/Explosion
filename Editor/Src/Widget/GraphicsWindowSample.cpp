//
// Created by Kindem on 2025/3/16.
//

#include <Editor/Widget/GraphicsWindowSample.h>
#include <Editor/Widget/moc_GraphicsWindowSample.cpp>

namespace Editor {
    GraphicsWindowSample::GraphicsWindowSample(QWindow* inParent)
        : GraphicsWindow(GetGraphicsWindowDesc(), inParent)
        , imageReadySemaphore(GetDevice().CreateSemaphore())
        , renderFinishedSemaphore(GetDevice().CreateSemaphore())
        , frameFence(GetDevice().CreateFence(true))
    {
        for (auto i = 0; i < swapChainTextureNum; i++) {
            swapChainTextures[i] = GetSwapChain().GetTexture(i);
            swapChainTextureViews[i] = swapChainTextures[i]->CreateTextureView(
                RHI::TextureViewCreateInfo()
                    .SetDimension(RHI::TextureViewDimension::tv2D)
                    .SetMipLevels(0, 1)
                    .SetArrayLayers(0, 1)
                    .SetAspect(RHI::TextureAspect::color)
                    .SetType(RHI::TextureViewType::colorAttachment));
        }
    }

    GraphicsWindowDesc GraphicsWindowSample::GetGraphicsWindowDesc()
    {
        GraphicsWindowDesc result;
        result.textureNum = swapChainTextureNum;
        result.formatQualifiers = { RHI::PixelFormat::rgba8Unorm, RHI::PixelFormat::bgra8Unorm };
        result.presentMode = RHI::PresentMode::immediately;
        return result;
    }

    GraphicsWindowSampleWidget::GraphicsWindowSampleWidget(QWidget* inParent)
        : QWidget(inParent)
        , graphicsWindow(new GraphicsWindowSample())
    {
        graphicsWindowContainer = createWindowContainer(graphicsWindow);
        graphicsWindowContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        graphicsWindowContainer->show();

        mainLayout = new QVBoxLayout();
        setLayout(mainLayout);

        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->addWidget(graphicsWindowContainer);
    }
} // namespace Editor
