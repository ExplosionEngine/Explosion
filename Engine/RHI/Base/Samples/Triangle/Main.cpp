//
// Created by John Kindem on 2021/4/4 0004.
//

#include <Application/Application.h>
#include <IO/FileManager.h>
#include <RHI/Driver.h>

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
using namespace RHI;

namespace {
    struct Vector3 {
        float v[3];
    };

    struct Vertex {
        Vector3 position;
        Vector3 color;
    };

    struct UboData {
        float aspect;
        float angle;
    };

    using Index = uint32_t;

    const uint32_t width = 1024;
    const uint32_t height = 768;
}

class App : public Application {
public:
    App(const std::string& name, uint32_t width, uint32_t height) : Application(name, width, height) {}

protected:
    void OnStart() override
    {
        driver = std::unique_ptr<Driver>(DriverFactory::Singleton().CreateDriverBySuggestion());

        SwapChain::Config swapChainConfig {};
        swapChainConfig.width = GetWidth();
        swapChainConfig.height = GetHeight();
#ifdef TARGET_OS_MAC
        swapChainConfig.surface = static_cast<void*>(glfwGetCocoaWindow(GetWindow()));
#endif
#ifdef _WIN32
        swapChainConfig.surface = static_cast<void*>(glfwGetWin32Window(GetWindow()));
#endif
        swapChain = driver->CreateSwapChain(swapChainConfig);

        ImageView::Config imageViewConfig {};
        imageViewConfig.type = ImageViewType::VIEW_2D;
        imageViewConfig.mipLevelCount = 1;
        imageViewConfig.baseMipLevel = 0;
        imageViewConfig.layerCount = 1;
        imageViewConfig.baseLayer = 0;
        imageViewConfig.aspects = FlagsCast(ImageAspectBits::COLOR);
        imageViews.resize(swapChain->GetColorAttachmentCount());
        for (uint32_t i = 0; i < imageViews.size(); i++) {
            imageViewConfig.image = swapChain->GetColorAttachment(i);
            imageViews[i] = driver->CreateImageView(imageViewConfig);
        }
        
        RenderPass::Config renderPassConfig {};
        renderPassConfig.attachmentConfigs = {{
            AttachmentType::SWAP_CHAIN_COLOR_ATTACHMENT,
            swapChain->GetSurfaceFormat(),
            AttachmentLoadOp::CLEAR,
            AttachmentStoreOp::STORE
        }};
        renderPass = driver->CreateRenderPass(renderPassConfig);

        FrameBuffer::Config frameBufferConfig {};
        frameBufferConfig.renderPass = renderPass;
        frameBufferConfig.width = GetWidth();
        frameBufferConfig.height = GetHeight();
        frameBufferConfig.layers = 1;
        frameBuffers.resize(swapChain->GetColorAttachmentCount());
        for (uint32_t i = 0; i < frameBuffers.size(); i++) {
            frameBufferConfig.attachments = { imageViews[i] };
            frameBuffers[i] = driver->CreateFrameBuffer(frameBufferConfig);
        }

        Shader::Config vsConfig = {
            ShaderStageBits::VERTEX,   IO::FileManager::ReadFile("RHI-Triangle-Vertex.spv", true)
        };
        Shader::Config fsConfig = {
            ShaderStageBits::FRAGMENT, IO::FileManager::ReadFile("RHI-Triangle-Fragment.spv", true)
        };
        vs = driver->CreateShader(vsConfig);
        fs = driver->CreateShader(fsConfig);

        GraphicsPipeline::Config pipelineConfig {};
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.shaderConfig.shaderModules = {vs, fs};
        pipelineConfig.vertexConfig.vertexBindings = {
            { 0, sizeof(Vertex), VertexInputRate::PER_VERTEX }
        };
        pipelineConfig.vertexConfig.vertexAttributes = {
            {0, 0, Format::R32_G32_B32_FLOAT, static_cast<uint32_t>(offsetof(Vertex, position))},
            {0, 1, Format::R32_G32_B32_FLOAT, static_cast<uint32_t>(offsetof(Vertex, color))}
        };
        pipelineConfig.descriptorConfig.descriptorAttributes = {
            {0, DescriptorType::UNIFORM_BUFFER, (uint32_t)ShaderStageBits::VERTEX},
        };
        pipelineConfig.viewportScissorConfig.viewport = { 0, 0, static_cast<float>(GetWidth()), static_cast<float>(GetHeight()), 0, 1.0 };
        pipelineConfig.viewportScissorConfig.scissor = { 0, 0, GetWidth(), GetHeight() };
        pipelineConfig.rasterizerConfig = { false, false, FlagsCast(CullModeBits::NONE), FrontFace::CLOCK_WISE };
        pipelineConfig.depthStencilConfig = { false, false, false };
        pipelineConfig.colorBlendConfig.enabled = false;
        pipelineConfig.assemblyConfig.topology = PrimitiveTopology::TRIANGLE_LIST;
        pipeline = driver->CreateGraphicsPipeline(pipelineConfig);

        float length = 0.5f;
        float s60 = length * sin(60.f / 180.f * 3.14f);
        float c60 = length * cos(60.f / 180.f * 3.14f);
        vertices = {
            { {0.f, length, 0.f}, {1.f, 0.f, 0.f} },
            { {-s60, -c60, .0f}, {0.f, 1.f, 0.f} },
            { {s60, -c60, 0.f}, {0.f, 0.f, 1.f} },
        };
        indices = {
            0, 1, 2
        };
        Buffer::Config bufferConfig {};
        bufferConfig.size = sizeof(Vertex) * vertices.size();
        bufferConfig.usages = BufferUsageBits::VERTEX_BUFFER | BufferUsageBits::TRANSFER_DST;
        bufferConfig.memoryProperties = FlagsCast(MemoryPropertyBits::DEVICE_LOCAL);
        vertexBuffer = driver->CreateBuffer(bufferConfig);
        vertexBuffer->UpdateData(vertices.data());

        bufferConfig.size = sizeof(Index) * indices.size();
        bufferConfig.usages = BufferUsageBits::INDEX_BUFFER | BufferUsageBits::TRANSFER_DST;
        bufferConfig.memoryProperties = FlagsCast(MemoryPropertyBits::DEVICE_LOCAL);
        indexBuffer = driver->CreateBuffer(bufferConfig);
        indexBuffer->UpdateData(indices.data());

        bufferConfig.size = sizeof(UboData);
        bufferConfig.usages = BufferUsageBits::UNIFORM_BUFFER | BufferUsageBits::TRANSFER_DST;
        bufferConfig.memoryProperties = MemoryPropertyBits::HOST_VISIBLE | MemoryPropertyBits::HOST_COHERENT;
        ubo = driver->CreateBuffer(bufferConfig);

        data.aspect = GetWidth() / (float)GetHeight();
        data.angle = 0.f;
        ubo->UpdateData(&data);

        DescriptorPool::Config poolConfig = {};
        poolConfig.maxSets = 100;
        poolConfig.poolSizes = {
            {DescriptorType::UNIFORM_BUFFER, 100},
        };
        pool = driver->CreateDescriptorPool(poolConfig);
        set = driver->AllocateDescriptorSet(pool, pipeline);

        DescriptorSet::DescriptorBufferInfo dbi = {};
        dbi.buffer = ubo;
        dbi.offset = 0;
        dbi.range = sizeof(UboData);
        set->WriteDescriptors({{ 0, DescriptorType::UNIFORM_BUFFER, &dbi, nullptr}});

        commandBuffer = driver->CreateCommandBuffer();
    }

    void OnStop() override
    {
        driver->DestroyCommandBuffer(commandBuffer);
        driver->DestroyBuffer(vertexBuffer);
        driver->DestroyBuffer(indexBuffer);
        driver->DestroyBuffer(ubo);
        driver->DestroyDescriptorPool(pool);
        driver->DestroyGraphicsPipeline(pipeline);
        driver->DestroyShader(vs);
        driver->DestroyShader(fs);
        for (auto* frameBuffer : frameBuffers) {
            driver->DestroyFrameBuffer(frameBuffer);
        }
        driver->DestroyRenderPass(renderPass);
        for (auto* imageView : imageViews) {
            driver->DestroyImageView(imageView);
        }
        driver->DestroySwapChain(swapChain);
    }

    void OnDrawFrame() override
    {
        swapChain->DoFrame([this](uint32_t imageIdx, Signal* imageReadySignal, Signal* frameFinishedSignal) -> void {
            ubo->UpdateData(&data);
            data.angle += 0.01f;
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
                    encoder->BindDescriptorSet({set});
                    encoder->DrawIndexed(0, indices.size(), 0, 0, 1);
                }
                encoder->EndRenderPass();
            });
            commandBuffer->Submit(imageReadySignal, frameFinishedSignal, FlagsCast(PipelineStageBits::COLOR_ATTACHMENT_OUTPUT));
        });
    }

private:
    UboData data = {};
    std::vector<Vertex> vertices;
    std::vector<Index> indices;

    std::unique_ptr<Driver> driver;
    SwapChain* swapChain = nullptr;
    std::vector<ImageView*> imageViews;
    RenderPass* renderPass = nullptr;
    std::vector<FrameBuffer*> frameBuffers;
    GraphicsPipeline* pipeline = nullptr;
    Buffer* vertexBuffer = nullptr;
    Buffer* indexBuffer = nullptr;
    Buffer* ubo = nullptr;
    Shader* vs = nullptr;
    Shader* fs = nullptr;
    CommandBuffer* commandBuffer = nullptr;
    DescriptorPool* pool = nullptr;
    DescriptorSet* set = nullptr;
};

int main(int argc, char* argv[])
{
    App app("Triangle", width, height);
    app.Run();
    return 0;
}
