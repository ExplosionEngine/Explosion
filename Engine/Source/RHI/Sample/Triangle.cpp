//
// Created by johnk on 9/1/2022.
//

#include <vector>
#include <array>

#include <glm/glm.hpp>

#include <Application.h>
using namespace RHI;

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

class TriangleApplication : public Application {
public:
    NON_COPYABLE(TriangleApplication)
    explicit TriangleApplication(const std::string& n) : Application(n) {}
    ~TriangleApplication() override = default;

protected:
    void OnCreate() override
    {
        CreateInstanceAndSelectGPU();
        RequestDeviceAndFetchQueues();
        CreateSwapChain();
        CreateVertexBuffer();
        CreatePipelineLayout();
        CreatePipeline();
        CreateFence();
    }

    void OnDrawFrame() override
    {
        CreateCommandBuffer();
        SubmitCommandBufferAndPresent();
        WaitPreviousFrame();
    }

    void OnDestroy() override
    {
        fence->Destroy();
        commandBuffer->Destroy();
        pipeline->Destroy();
        pipelineLayout->Destroy();
        for (auto* textureView : swapChainTextureViews) {
            textureView->Destroy();
        }
        vertexBufferView->Destroy();
        vertexBuffer->Destroy();
        swapChain->Destroy();
        device->Destroy();
        instance->Destroy();
    }

private:
    void CreateInstanceAndSelectGPU()
    {
        instance = Instance::CreateByType(rhiType);

        gpu = instance->GetGpu(0);
    }

    void RequestDeviceAndFetchQueues()
    {
        std::vector<QueueInfo> queueCreateInfos = {{ QueueType::GRAPHICS, 1 }};
        DeviceCreateInfo createInfo {};
        createInfo.queueCreateInfoNum = queueCreateInfos.size();
        createInfo.queueCreateInfos = queueCreateInfos.data();
        device = gpu->RequestDevice(&createInfo);
        graphicsQueue = device->GetQueue(QueueType::GRAPHICS, 0);
    }

    void CreateSwapChain()
    {
        SwapChainCreateInfo swapChainCreateInfo {};
        swapChainCreateInfo.format = PixelFormat::RGBA8_UNORM;
        swapChainCreateInfo.presentMode = PresentMode::IMMEDIATELY;
        swapChainCreateInfo.textureNum = 2;
        swapChainCreateInfo.extent = { width, height };
        swapChainCreateInfo.window = GetPlatformWindow();
        swapChainCreateInfo.presentQueue = graphicsQueue;
        swapChain = device->CreateSwapChain(&swapChainCreateInfo);

        for (auto i = 0; i < 2; i++) {
            swapChainTextures[i] = swapChain->GetTexture(i);

            TextureViewCreateInfo viewCreateInfo {};
            viewCreateInfo.format = PixelFormat::RGBA8_UNORM;
            viewCreateInfo.dimension = TextureViewDimension::TV_2D;
            viewCreateInfo.baseArrayLayer = 0;
            viewCreateInfo.arrayLayerNum = 1;
            viewCreateInfo.baseMipLevel = 0;
            viewCreateInfo.mipLevelNum = 1;
            viewCreateInfo.aspect = TextureAspect::COLOR;
            swapChainTextureViews[i] = swapChainTextures[i]->CreateTextureView(&viewCreateInfo);
        }
    }

    void CreateVertexBuffer()
    {
        std::vector<Vertex> vertices = {
            { { -.5f, -.5f, 0.f }, { 1.f, 0.f, 0.f } },
            { { .5f, -.5f, 0.f }, { 0.f, 1.f, 0.f } },
            { { 0.f, .5f, 0.f }, { 0.f, 0.f, 1.f } },
        };

        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = vertices.size() * sizeof(Vertex);
        bufferCreateInfo.usages = BufferUsageBits::VERTEX | BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        vertexBuffer = device->CreateBuffer(&bufferCreateInfo);
        if (vertexBuffer != nullptr) {
            auto* data = vertexBuffer->Map(MapMode::WRITE, 0, bufferCreateInfo.size);
            memcpy(data, vertices.data(), bufferCreateInfo.size);
            vertexBuffer->UnMap();
        }

        BufferViewCreateInfo bufferViewCreateInfo {};
        bufferViewCreateInfo.size = vertices.size() * sizeof(Vertex);
        bufferViewCreateInfo.offset = 0;
        bufferViewCreateInfo.vertex.stride = sizeof(Vertex);
        vertexBufferView = vertexBuffer->CreateBufferView(&bufferViewCreateInfo);
    }

    void CreatePipelineLayout()
    {
        PipelineLayoutCreateInfo createInfo {};
        createInfo.bindGroupNum = 0;
        createInfo.bindGroupLayouts = nullptr;
        pipelineLayout = device->CreatePipelineLayout(&createInfo);
    }

    void CreatePipeline()
    {
        ShaderModule* vertexShader;
        ShaderModule* fragmentShader;

        std::vector<uint8_t> vsByteCode;
        CompileShader(vsByteCode, "Shader/Sample/Triangle.hlsl", "VSMain", RHI::ShaderStageBits::VERTEX);

        ShaderModuleCreateInfo shaderModuleCreateInfo {};
        shaderModuleCreateInfo.size = vsByteCode.size();
        shaderModuleCreateInfo.byteCode = vsByteCode.data();
        vertexShader = device->CreateShaderModule(&shaderModuleCreateInfo);

        std::vector<uint8_t> fsByteCode;
        CompileShader(fsByteCode, "Shader/Sample/Triangle.hlsl", "FSMain", RHI::ShaderStageBits::FRAGMENT);

        shaderModuleCreateInfo.size = fsByteCode.size();
        shaderModuleCreateInfo.byteCode = fsByteCode.data();
        fragmentShader = device->CreateShaderModule(&shaderModuleCreateInfo);

        std::array<VertexAttribute, 2> vertexAttributes {};
        vertexAttributes[0].format = VertexFormat::FLOAT32_X3;
        vertexAttributes[0].offset = 0;
        vertexAttributes[0].semanticName = "POSITION";
        vertexAttributes[0].semanticIndex = 0;
        vertexAttributes[1].format = VertexFormat::FLOAT32_X3;
        vertexAttributes[1].offset = offsetof(Vertex, color);
        vertexAttributes[1].semanticName = "COLOR";
        vertexAttributes[1].semanticIndex = 0;

        VertexBufferLayout vertexBufferLayout {};
        vertexBufferLayout.stepMode = RHI::VertexStepMode::PER_VERTEX;
        vertexBufferLayout.stride = sizeof(Vertex);
        vertexBufferLayout.attributeNum = vertexAttributes.size();
        vertexBufferLayout.attributes = vertexAttributes.data();

        std::array<ColorTargetState, 1> colorTargetStates {};
        colorTargetStates[0].format = PixelFormat::RGBA8_UNORM;
        colorTargetStates[0].writeFlags = ColorWriteBits::RED | ColorWriteBits::GREEN | ColorWriteBits::BLUE | ColorWriteBits::ALPHA;

        GraphicsPipelineCreateInfo createInfo {};
        createInfo.vertexShader = vertexShader;
        createInfo.fragmentShader = fragmentShader;
        createInfo.layout = pipelineLayout;
        createInfo.vertex.bufferLayoutNum = 1;
        createInfo.vertex.bufferLayouts = &vertexBufferLayout;
        createInfo.fragment.colorTargetNum = colorTargetStates.size();
        createInfo.fragment.colorTargets = colorTargetStates.data();
        createInfo.primitive.depthClip = false;
        createInfo.primitive.frontFace = RHI::FrontFace::CCW;
        createInfo.primitive.cullMode = CullMode::NONE;
        createInfo.primitive.topologyType = RHI::PrimitiveTopologyType::TRIANGLE;
        createInfo.primitive.stripIndexFormat = IndexFormat::UINT16;
        createInfo.depthStencil.depthEnable = false;
        createInfo.depthStencil.stencilEnable = false;
        createInfo.multiSample.count = 1;
        pipeline = device->CreateGraphicsPipeline(&createInfo);
    }

    void CreateFence()
    {
        fence = device->CreateFence();
    }

    void CreateCommandBuffer()
    {
        commandBuffer = device->CreateCommandBuffer();
        CommandEncoder* commandEncoder = commandBuffer->Begin();
        {
            std::array<GraphicsPassColorAttachment, 1> colorAttachments {};
            colorAttachments[0].clearValue = ColorNormalized<4> { 0.0f, 0.0f, 0.0f, 1.0f };
            colorAttachments[0].loadOp = LoadOp::CLEAR;
            colorAttachments[0].storeOp = StoreOp::STORE;
            colorAttachments[0].view = swapChainTextureViews[swapChain->GetBackTextureIndex()];
            colorAttachments[0].resolveTarget = nullptr;

            GraphicsPassBeginInfo graphicsPassBeginInfo {};
            graphicsPassBeginInfo.colorAttachmentNum = colorAttachments.size();
            graphicsPassBeginInfo.colorAttachments = colorAttachments.data();
            graphicsPassBeginInfo.depthStencilAttachment = nullptr;

            commandEncoder->ResourceBarrier(Barrier::Transition(swapChainTextures[swapChain->GetBackTextureIndex()], TextureState::PRESENT, TextureState::RENDER_TARGET));
            auto* graphicsEncoder = commandEncoder->BeginGraphicsPass(&graphicsPassBeginInfo);
            {
                graphicsEncoder->SetPipeline(pipeline);
                graphicsEncoder->SetScissor(0, 0, width, height);
                graphicsEncoder->SetViewport(0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1);
                graphicsEncoder->SetPrimitiveTopology(PrimitiveTopology::TRIANGLE_LIST);
                graphicsEncoder->SetVertexBuffer(0, vertexBufferView);
                graphicsEncoder->Draw(3, 1, 0, 0);
            }
            graphicsEncoder->EndPass();
            commandEncoder->ResourceBarrier(Barrier::Transition(swapChainTextures[swapChain->GetBackTextureIndex()], TextureState::RENDER_TARGET, TextureState::PRESENT));
        }
        commandEncoder->End();
    }

    void SubmitCommandBufferAndPresent()
    {
        graphicsQueue->Submit(commandBuffer, fence);
        swapChain->Present();
    }

    void WaitPreviousFrame()
    {
        fence->Wait();
    }

    Instance* instance = nullptr;
    Gpu* gpu = nullptr;
    Device* device = nullptr;
    Queue* graphicsQueue = nullptr;
    SwapChain* swapChain = nullptr;
    Buffer* vertexBuffer = nullptr;
    BufferView* vertexBufferView = nullptr;
    std::array<Texture*, 2> swapChainTextures {};
    std::array<TextureView*, 2> swapChainTextureViews {};
    PipelineLayout* pipelineLayout = nullptr;
    GraphicsPipeline* pipeline = nullptr;
    CommandBuffer* commandBuffer = nullptr;
    Fence* fence = nullptr;
};

int main(int argc, char* argv[])
{
    TriangleApplication application("RHI-Triangle");
    return application.Run(argc, argv);
}
