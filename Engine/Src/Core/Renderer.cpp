//
// Created by Administrator on 2021/4/3 0003.
//

#include <Explosion/Core/Renderer.h>
#include <Explosion/Driver/EnumAdapter.h>
#include <Explosion/Driver/Image.h>
#include <Explosion/Driver/Driver.h>
#include <Explosion/Driver/SwapChain.h>
#include <Explosion/Driver/RenderPass.h>
#include <Explosion/Driver/ImageView.h>
#include <Explosion/Driver/FrameBuffer.h>

namespace Explosion {
    Renderer::Renderer(Driver& driver, void* surface, uint32_t width, uint32_t height)
        : driver(driver), surface(surface), width(width), height(height)
    {
        CreateSwapChain();
    }

    Renderer::~Renderer()
    {
        DestroySwapChain();
    }

    void Renderer::CreateSwapChain()
    {
        swapChain = driver.CreateGpuRes<SwapChain>(surface, width, height);

        ImageView::Config imageConfig {
            ImageViewType::VIEW_2D,
            1, 0,
            1, 0
        };
        colorAttachmentViews.resize(swapChain->GetColorAttachmentCount());
        for (uint32_t i = 0; i < colorAttachmentViews.size(); i++) {
            colorAttachmentViews[i] = driver.CreateGpuRes<ImageView>(swapChain->GetColorAttachments()[i], imageConfig);
        }

        RenderPass::Config renderPassConfig {};
        renderPassConfig.attachmentConfigs.emplace_back(RenderPass::AttachmentConfig {
            AttachmentType::SWAP_CHAIN_COLOR_ATTACHMENT,
            GetEnumByVk<VkFormat, Format>(swapChain->GetVkSurfaceFormat().format),
            AttachmentLoadOp::CLEAR,
            AttachmentStoreOp::STORE
        });
        renderPass = driver.CreateGpuRes<RenderPass>(renderPassConfig);

        frameBuffers.resize(colorAttachmentViews.size());
        for (uint32_t i = 0; i < frameBuffers.size(); i++) {
            FrameBuffer::Config frameBufferConfig {
                swapChain->GetVkExtent().width, swapChain->GetVkExtent().height, 1,
                { colorAttachmentViews[i] }
            };
            frameBuffers[i] = driver.CreateGpuRes<FrameBuffer>(renderPass, frameBufferConfig);
        }
    }

    void Renderer::DestroySwapChain()
    {
        for (auto& frameBuffer : frameBuffers) {
            driver.DestroyGpuRes<FrameBuffer>(frameBuffer);
        }
        driver.DestroyGpuRes<RenderPass>(renderPass);
        for (auto& imageView : colorAttachmentViews) {
            driver.DestroyGpuRes<ImageView>(imageView);
        }
        driver.DestroyGpuRes<SwapChain>(swapChain);
    }
}
