//
// Created by Administrator on 2021/4/4 0004.
//

#include <Application/Application.h>
#include <Explosion/RHI/Driver.h>
#include <Explosion/RHI/SwapChain.h>
#include <Explosion/RHI/RenderPass.h>
#include <Explosion/RHI/FrameBuffer.h>
#include <Explosion/RHI/Pipeline.h>
#include <Explosion/RHI/Image.h>
#include <Explosion/RHI/ImageView.h>
#include <Explosion/RHI/GpuBuffer.h>
#include <Explosion/RHI/Signal.h>
#include <Explosion/RHI/CommandBuffer.h>
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
        driver = std::make_unique<Driver>();

#ifdef TARGET_OS_MAC
        swapChain = driver->CreateGpuRes<SwapChain>(glfwGetCocoaWindow(GetWindow()), GetWidth(), GetHeight());
#endif
#ifdef _WIN32
        swapChain = driver->CreateGpuRes<SwapChain>(glfwGetWin32Window(GetWindow()), GetWidth(), GetHeight());
#endif

        ImageView::Config imageViewConfig {};
        imageViewConfig.type = ImageViewType::VIEW_2D;
        imageViewConfig.mipLevelCount = 1;
        imageViewConfig.baseMipLevel = 0;
        imageViewConfig.layerCount = 1;
        imageViewConfig.baseLayer = 0;
        imageViews.resize(swapChain->GetColorAttachmentCount());
        for (uint32_t i = 0; i < imageViews.size(); i++) {
            imageViewConfig.image = swapChain->GetColorAttachments()[i];
            imageViews[i] = driver->CreateGpuRes<ImageView>(imageViewConfig);
        }
        
        RenderPass::Config renderPassConfig {};
        renderPassConfig.attachmentConfigs = {{
            AttachmentType::SWAP_CHAIN_COLOR_ATTACHMENT,
            swapChain->GetSurfaceFormat(),
            AttachmentLoadOp::CLEAR,
            AttachmentStoreOp::STORE
        }};
        renderPass = driver->CreateGpuRes<RenderPass>(renderPassConfig);

        FrameBuffer::Config frameBufferConfig {};
        frameBufferConfig.renderPass = renderPass;
        frameBufferConfig.width = GetWidth();
        frameBufferConfig.height = GetHeight();
        frameBufferConfig.layers = 1;
        frameBuffers.resize(swapChain->GetColorAttachmentCount());
        for (uint32_t i = 0; i < frameBuffers.size(); i++) {
            frameBufferConfig.attachments = { imageViews[i] };
            frameBuffers[i] = driver->CreateGpuRes<FrameBuffer>(frameBufferConfig);
        }

        GraphicsPipeline::Config pipelineConfig {};
        pipelineConfig.shaderConfig.shaderModules = {
            { ShaderStage::VERTEX, FileReader::Read("1-Triangle-Vertex.spv") },
            { ShaderStage::FRAGMENT, FileReader::Read("1-Triangle-Fragment.spv") }
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
        pipelineConfig.rasterizerConfig = { false, false, CullMode::NONE, FrontFace::CLOCK_WISE };
        pipelineConfig.depthStencilConfig = { false, false, false };
        pipelineConfig.colorBlendConfig.enabled = false;
        pipeline = driver->CreateGpuRes<GraphicsPipeline>(renderPass, pipelineConfig);

        vertices = {
            { 0.f, -.5f, 0.f },
            { .5f, .5f, .0f },
            { -.5f, .5f, 0.f }
        };
        indices = {
            0, 1, 2
        };
        GpuBuffer::Config bufferConfig {};
        bufferConfig.size = sizeof(Vertex) * vertices.size();
        bufferConfig.usages = { BufferUsage::VERTEX_BUFFER };
        vertexBuffer = driver->CreateGpuRes<DeviceLocalBuffer>(bufferConfig);
        vertexBuffer->UpdateData(vertices.data());

        bufferConfig.size = sizeof(Index) * indices.size();
        bufferConfig.usages = { BufferUsage::INDEX_BUFFER };
        indexBuffer = driver->CreateGpuRes<DeviceLocalBuffer>(bufferConfig);
        indexBuffer->UpdateData(indices.data());
    }

    void OnStop() override
    {
        driver->DestroyGpuRes<GpuBuffer>(vertexBuffer);
        driver->DestroyGpuRes<GpuBuffer>(indexBuffer);
        driver->DestroyGpuRes<Pipeline>(pipeline);
        for (auto* frameBuffer : frameBuffers) {
            driver->DestroyGpuRes<FrameBuffer>(frameBuffer);
        }
        driver->DestroyGpuRes<RenderPass>(renderPass);
        for (auto* imageView : imageViews) {
            driver->DestroyGpuRes<ImageView>(imageView);
        }
        driver->DestroyGpuRes<SwapChain>(swapChain);
    }

    void OnDrawFrame() override
    {
        swapChain->DoFrame([this](uint32_t imageIdx, Signal* imageReadySignal, Signal* frameFinishedSignal) -> void {
            auto* commandBuffer = driver->CreateGpuRes<FrameOutputCommandBuffer>();
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
            commandBuffer->Submit(imageReadySignal, frameFinishedSignal);
        });
    }

private:
    std::vector<Vertex> vertices;
    std::vector<Index> indices;

    std::unique_ptr<Driver> driver;
    SwapChain* swapChain = nullptr;
    std::vector<ImageView*> imageViews;
    RenderPass* renderPass = nullptr;
    std::vector<FrameBuffer*> frameBuffers;
    Pipeline* pipeline = nullptr;
    GpuBuffer* vertexBuffer = nullptr;
    GpuBuffer* indexBuffer = nullptr;
};

int main(int argc, char* argv[])
{
    App app("Triangle", 1024, 768);
    app.Run();
    return 0;
}
