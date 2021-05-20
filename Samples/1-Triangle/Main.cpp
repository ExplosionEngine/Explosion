//
// Created by Administrator on 2021/4/4 0004.
//

#include <Application/Application.h>
#include <Explosion/RHI/Vulkan/VulkanDriver.h>
#include <Explosion/RHI/Vulkan/VulkanSwapChain.h>
#include <Explosion/RHI/Vulkan/VulkanRenderPass.h>
#include <Explosion/RHI/Vulkan/VulkanFrameBuffer.h>
#include <Explosion/RHI/Vulkan/VulkanGraphicsPipeline.h>
#include <Explosion/RHI/Vulkan/VulkanImage.h>
#include <Explosion/RHI/Vulkan/VulkanImageView.h>
#include <Explosion/RHI/Vulkan/VulkanBuffer.h>
#include <Explosion/RHI/Vulkan/VulkanSignal.h>
#include <Explosion/RHI/Vulkan/VulkanCommandBuffer.h>
#include <Explosion/Common/FileReader.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#ifdef TARGET_OS_MAC
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

using namespace Explosion;
using namespace Explosion::RHI;

namespace {
    struct Vertex {
        float position[3];
    };

    using Index = uint32_t;
}

class App : public Application {
public:
    App(const std::string& name, uint32_t width, uint32_t height) : Application(name, width, height) {}

protected:
    void OnStart() override
    {
        driver = std::make_unique<VulkanDriver>();

#ifdef TARGET_OS_MAC
        swapChain = driver->CreateGpuRes<VulkanSwapChain>(glfwGetCocoaWindow(GetWindow()), GetWidth(), GetHeight());
#endif
#ifdef _WIN32
        swapChain = driver->CreateGpuRes<VulkanSwapChain>(glfwGetWin32Window(GetWindow()), GetWidth(), GetHeight());
#endif

        VulkanImageView::Config imageViewConfig {};
        imageViewConfig.type = ImageViewType::VIEW_2D;
        imageViewConfig.mipLevelCount = 1;
        imageViewConfig.baseMipLevel = 0;
        imageViewConfig.layerCount = 1;
        imageViewConfig.baseLayer = 0;
        imageViewConfig.aspects = FlagsCast(ImageAspectBits::COLOR);
        imageViews.resize(swapChain->GetColorAttachmentCount());
        for (uint32_t i = 0; i < imageViews.size(); i++) {
            imageViewConfig.image = swapChain->GetColorAttachments()[i];
            imageViews[i] = driver->CreateGpuRes<VulkanImageView>(imageViewConfig);
        }
        
        VulkanRenderPass::Config renderPassConfig {};
        renderPassConfig.attachmentConfigs = {{
            AttachmentType::SWAP_CHAIN_COLOR_ATTACHMENT,
            swapChain->GetSurfaceFormat(),
            AttachmentLoadOp::CLEAR,
            AttachmentStoreOp::STORE
        }};
        renderPass = driver->CreateGpuRes<VulkanRenderPass>(renderPassConfig);

        VulkanFrameBuffer::Config frameBufferConfig {};
        frameBufferConfig.renderPass = renderPass;
        frameBufferConfig.width = GetWidth();
        frameBufferConfig.height = GetHeight();
        frameBufferConfig.layers = 1;
        frameBuffers.resize(swapChain->GetColorAttachmentCount());
        for (uint32_t i = 0; i < frameBuffers.size(); i++) {
            frameBufferConfig.attachments = { imageViews[i] };
            frameBuffers[i] = driver->CreateGpuRes<VulkanFrameBuffer>(frameBufferConfig);
        }

        VulkanGraphicsPipeline::Config pipelineConfig {};
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.shaderConfig.shaderModules = {
            {ShaderStageBits::VERTEX,   FileReader::Read("1-Triangle-Vertex.spv") },
            {ShaderStageBits::FRAGMENT, FileReader::Read("1-Triangle-Fragment.spv") }
        };
        pipelineConfig.vertexConfig.vertexBindings = {
            { 0, sizeof(Vertex), VertexInputRate::PER_VERTEX }
        };
        pipelineConfig.vertexConfig.vertexAttributes = {
            { 0, 0, Format::R32_G32_B32_FLOAT, static_cast<uint32_t>(offsetof(Vertex, position)) },
        };
        pipelineConfig.descriptorConfig.descriptorPoolSizes = {};
        pipelineConfig.descriptorConfig.descriptorAttributes = {};
        pipelineConfig.descriptorConfig.maxSets = 0;
        pipelineConfig.viewportScissorConfig.viewport = { 0, 0, static_cast<float>(GetWidth()), static_cast<float>(GetHeight()), 0, 1.0 };
        pipelineConfig.viewportScissorConfig.scissor = { 0, 0, GetWidth(), GetHeight() };
        pipelineConfig.rasterizerConfig = { false, false, FlagsCast(CullModeBits::NONE), FrontFace::CLOCK_WISE };
        pipelineConfig.depthStencilConfig = { false, false, false };
        pipelineConfig.colorBlendConfig.enabled = false;
        pipeline = driver->CreateGpuRes<VulkanGraphicsPipeline>(pipelineConfig);

        vertices = {
            { 0.f, -.5f, 0.f },
            { .5f, .5f, .0f },
            { -.5f, .5f, 0.f }
        };
        indices = {
            0, 1, 2
        };
        VulkanBuffer::Config bufferConfig {};
        bufferConfig.size = sizeof(Vertex) * vertices.size();
        bufferConfig.usages = BufferUsageBits::VERTEX_BUFFER | BufferUsageBits::TRANSFER_DST;
        bufferConfig.memoryProperties = FlagsCast(MemoryPropertyBits::DEVICE_LOCAL);
        vertexBuffer = driver->CreateGpuRes<VulkanBuffer>(bufferConfig);
        vertexBuffer->UpdateData(vertices.data());

        bufferConfig.size = sizeof(Index) * indices.size();
        bufferConfig.usages = BufferUsageBits::INDEX_BUFFER | BufferUsageBits::TRANSFER_DST;
        bufferConfig.memoryProperties = FlagsCast(MemoryPropertyBits::DEVICE_LOCAL);
        indexBuffer = driver->CreateGpuRes<VulkanBuffer>(bufferConfig);
        indexBuffer->UpdateData(indices.data());
    }

    void OnStop() override
    {
        driver->DestroyGpuRes<VulkanBuffer>(vertexBuffer);
        driver->DestroyGpuRes<VulkanBuffer>(indexBuffer);
        driver->DestroyGpuRes<VulkanGraphicsPipeline>(pipeline);
        for (auto* frameBuffer : frameBuffers) {
            driver->DestroyGpuRes<VulkanFrameBuffer>(frameBuffer);
        }
        driver->DestroyGpuRes<VulkanRenderPass>(renderPass);
        for (auto* imageView : imageViews) {
            driver->DestroyGpuRes<VulkanImageView>(imageView);
        }
        driver->DestroyGpuRes<VulkanSwapChain>(swapChain);
    }

    void OnDrawFrame() override
    {
        swapChain->DoFrame([this](uint32_t imageIdx, VulkanSignal* imageReadySignal, VulkanSignal* frameFinishedSignal) -> void {
            auto* commandBuffer = driver->CreateGpuRes<VulkanCommandBuffer>();
            commandBuffer->EncodeCommands([imageIdx, this](CommandEncoder* encoder) -> void {
                CommandEncoder::RenderPassBeginInfo beginInfo {};
                beginInfo.frameBuffer = frameBuffers[imageIdx];
                beginInfo.renderArea = { 0, 0, GetWidth(), GetHeight() };
                beginInfo.clearValue = { 0.f, 0.f, 0.f, 1.f };
                encoder->BeginRenderPass(renderPass, beginInfo);
                {
                    encoder->BindGraphicsPipeline(pipeline);
                    encoder->SetViewPort({ 0, 0, static_cast<float>(GetWidth()), static_cast<float>(GetHeight()), 0.f, 1.f });
                    encoder->SetScissor({ 0, 0, GetWidth(), GetHeight() });

                    encoder->BindVertexBuffer(0, vertexBuffer);
                    encoder->BindIndexBuffer(indexBuffer);
                    encoder->DrawIndexed(0, indices.size(), 0, 0, 1);
                }
                encoder->EndRenderPass();
            });
            commandBuffer->Submit(imageReadySignal, frameFinishedSignal, FlagsCast(PipelineStageBits::COLOR_ATTACHMENT_OUTPUT));
        });
    }

private:
    std::vector<Vertex> vertices;
    std::vector<Index> indices;

    std::unique_ptr<VulkanDriver> driver;
    VulkanSwapChain* swapChain = nullptr;
    std::vector<VulkanImageView*> imageViews;
    VulkanRenderPass* renderPass = nullptr;
    std::vector<VulkanFrameBuffer*> frameBuffers;
    VulkanGraphicsPipeline* pipeline = nullptr;
    VulkanBuffer* vertexBuffer = nullptr;
    VulkanBuffer* indexBuffer = nullptr;
};

int main(int argc, char* argv[])
{
    App app("Triangle", 1024, 768);
    app.Run();
    return 0;
}
