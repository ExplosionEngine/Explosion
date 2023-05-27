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

        CreatePipelineLayout();
        CreatePipeline();
        CreateVertexBuffer();

        CreateFence();
        CreateCommandBuffer();
    }

    void OnDrawFrame() override
    {
        PopulateCommandBuffer();
        SubmitCommandBufferAndPresent();
    }

    void OnDestroy() override
    {
        graphicsQueue->Wait(fence.Get());
        fence->Wait();
    }

private:
    static const uint8_t backBufferCount = 2;

    void CreateInstanceAndSelectGPU()
    {
        instance = Instance::GetByType(rhiType);

        gpu = instance->GetGpu(0);
    }

    void RequestDeviceAndFetchQueues()
    {
        std::vector<QueueInfo> queueCreateInfos = {{QueueType::graphics, 1}};
        DeviceCreateInfo createInfo {};
        createInfo.queueCreateInfoNum = queueCreateInfos.size();
        createInfo.queueCreateInfos = queueCreateInfos.data();
        device = gpu->RequestDevice(createInfo);
        graphicsQueue = device->GetQueue(QueueType::graphics, 0);
    }

    void CreateSwapChain()
    {
        static std::vector<PixelFormat> swapChainFormatQualifiers = {
            PixelFormat::rgba8Unorm,
            PixelFormat::bgra8Unorm
        };

        SurfaceCreateInfo surfaceCreateInfo {};
        surfaceCreateInfo.window = GetPlatformWindow();
        surface = device->CreateSurface(surfaceCreateInfo);

        for (auto format : swapChainFormatQualifiers) {
            if (device->CheckSwapChainFormatSupport(surface.Get(), format)) {
                swapChainFormat = format;
                break;
            }
        }
        Assert(swapChainFormat != PixelFormat::max);

        SwapChainCreateInfo swapChainCreateInfo {};
        swapChainCreateInfo.format = swapChainFormat;
        swapChainCreateInfo.presentMode = PresentMode::immediately;
        swapChainCreateInfo.textureNum = backBufferCount;
        swapChainCreateInfo.extent = {width, height};
        swapChainCreateInfo.surface = surface.Get();
        swapChainCreateInfo.presentQueue = graphicsQueue;
        swapChain = device->CreateSwapChain(swapChainCreateInfo);

        for (auto i = 0; i < swapChainCreateInfo.textureNum; i++) {
            swapChainTextures[i] = swapChain->GetTexture(i);

            TextureViewCreateInfo viewCreateInfo {};
            viewCreateInfo.dimension = TextureViewDimension::tv2D;
            viewCreateInfo.baseArrayLayer = 0;
            viewCreateInfo.arrayLayerNum = 1;
            viewCreateInfo.baseMipLevel = 0;
            viewCreateInfo.mipLevelNum = 1;
            viewCreateInfo.aspect = TextureAspect::color;
            viewCreateInfo.type = TextureViewType::colorAttachment;
            swapChainTextureViews[i] = swapChainTextures[i]->CreateTextureView(viewCreateInfo);
        }
    }

    void CreateVertexBuffer()
    {
        std::vector<Vertex> vertices = {
            {{-.5f, -.5f, 0.f}, {1.f, 0.f, 0.f}},
            {{.5f, -.5f, 0.f}, {0.f, 1.f, 0.f}},
            {{0.f, .5f, 0.f}, {0.f, 0.f, 1.f}},
        };

        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = vertices.size() * sizeof(Vertex);
        bufferCreateInfo.usages = BufferUsageBits::vertex | BufferUsageBits::mapWrite | BufferUsageBits::copySrc;
        vertexBuffer = device->CreateBuffer(bufferCreateInfo);
        if (vertexBuffer != nullptr) {
            auto* data = vertexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
            memcpy(data, vertices.data(), bufferCreateInfo.size);
            vertexBuffer->UnMap();
        }

        BufferViewCreateInfo bufferViewCreateInfo {};
        bufferViewCreateInfo.size = vertices.size() * sizeof(Vertex);
        bufferViewCreateInfo.offset = 0;
        bufferViewCreateInfo.vertex.stride = sizeof(Vertex);
        vertexBufferView = vertexBuffer->CreateBufferView(bufferViewCreateInfo);
    }

    void CreatePipelineLayout()
    {
        PipelineLayoutCreateInfo createInfo {};
        createInfo.bindGroupLayoutNum = 0;
        createInfo.bindGroupLayouts = nullptr;
        pipelineLayout = device->CreatePipelineLayout(createInfo);
    }

    void CreatePipeline()
    {
        std::vector<uint8_t> vsByteCode;
        CompileShader(vsByteCode, "Triangle/Triangle.hlsl", "VSMain", RHI::ShaderStageBits::sVertex);

        ShaderModuleCreateInfo shaderModuleCreateInfo {};
        shaderModuleCreateInfo.size = vsByteCode.size();
        shaderModuleCreateInfo.byteCode = vsByteCode.data();
        vertexShader = device->CreateShaderModule(shaderModuleCreateInfo);

        std::vector<uint8_t> fsByteCode;
        CompileShader(fsByteCode, "Triangle/Triangle.hlsl", "FSMain", RHI::ShaderStageBits::sPixel);

        shaderModuleCreateInfo.size = fsByteCode.size();
        shaderModuleCreateInfo.byteCode = fsByteCode.data();
        fragmentShader = device->CreateShaderModule(shaderModuleCreateInfo);

        std::array<VertexAttribute, 2> vertexAttributes {};
        vertexAttributes[0].format = VertexFormat::float32X3;
        vertexAttributes[0].offset = 0;
        vertexAttributes[0].semanticName = "POSITION";
        vertexAttributes[0].semanticIndex = 0;
        vertexAttributes[1].format = VertexFormat::float32X3;
        vertexAttributes[1].offset = offsetof(Vertex, color);
        vertexAttributes[1].semanticName = "COLOR";
        vertexAttributes[1].semanticIndex = 0;

        VertexBufferLayout vertexBufferLayout {};
        vertexBufferLayout.stepMode = RHI::VertexStepMode::perVertex;
        vertexBufferLayout.stride = sizeof(Vertex);
        vertexBufferLayout.attributeNum = vertexAttributes.size();
        vertexBufferLayout.attributes = vertexAttributes.data();

        std::array<ColorTargetState, 1> colorTargetStates {};
        colorTargetStates[0].format = swapChainFormat;
        colorTargetStates[0].writeFlags = ColorWriteBits::red | ColorWriteBits::green | ColorWriteBits::blue | ColorWriteBits::alpha;

        GraphicsPipelineCreateInfo createInfo {};
        createInfo.vertexShader = vertexShader.Get();
        createInfo.pixelShader = fragmentShader.Get();
        createInfo.layout = pipelineLayout.Get();
        createInfo.vertexState.bufferLayoutNum = 1;
        createInfo.vertexState.bufferLayouts = &vertexBufferLayout;
        createInfo.fragmentState.colorTargetNum = colorTargetStates.size();
        createInfo.fragmentState.colorTargets = colorTargetStates.data();
        createInfo.primitiveState.depthClip = false;
        createInfo.primitiveState.frontFace = RHI::FrontFace::ccw;
        createInfo.primitiveState.cullMode = CullMode::none;
        createInfo.primitiveState.topologyType = RHI::PrimitiveTopologyType::triangle;
        createInfo.primitiveState.stripIndexFormat = IndexFormat::uint16;
        createInfo.depthStencilState.depthEnable = false;
        createInfo.depthStencilState.stencilEnable = false;
        createInfo.multiSampleState.count = 1;
        pipeline = device->CreateGraphicsPipeline(createInfo);
    }

    void CreateFence()
    {
        fence = device->CreateFence();
    }

    void CreateCommandBuffer()
    {
        commandBuffer = device->CreateCommandBuffer();
    }

    void PopulateCommandBuffer()
    {
        auto backTextureIndex = swapChain->AcquireBackTexture();
        CommandEncoder* commandEncoder = commandBuffer->Begin();
        {
            std::array<GraphicsPassColorAttachment, 1> colorAttachments {};
            colorAttachments[0].clearValue = ColorNormalized<4> {0.0f, 0.0f, 0.0f, 1.0f};
            colorAttachments[0].loadOp = LoadOp::clear;
            colorAttachments[0].storeOp = StoreOp::store;
            colorAttachments[0].view = swapChainTextureViews[backTextureIndex].Get();
            colorAttachments[0].resolve = nullptr;

            GraphicsPassBeginInfo graphicsPassBeginInfo {};
            graphicsPassBeginInfo.colorAttachmentNum = colorAttachments.size();
            graphicsPassBeginInfo.colorAttachments = colorAttachments.data();
            graphicsPassBeginInfo.depthStencilAttachment = nullptr;

            commandEncoder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::present, TextureState::renderTarget));
            auto* graphicsEncoder = commandEncoder->BeginGraphicsPass(&graphicsPassBeginInfo);
            {
                graphicsEncoder->SetPipeline(pipeline.Get());
                graphicsEncoder->SetScissor(0, 0, width, height);
                graphicsEncoder->SetViewport(0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1);
                graphicsEncoder->SetPrimitiveTopology(PrimitiveTopology::triangleList);
                graphicsEncoder->SetVertexBuffer(0, vertexBufferView.Get());
                graphicsEncoder->Draw(3, 1, 0, 0);
            }
            graphicsEncoder->EndPass();
            commandEncoder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::renderTarget, TextureState::present));
        }
        commandEncoder->SwapChainSync(swapChain.Get());
        commandEncoder->End();
    }

    void SubmitCommandBufferAndPresent()
    {
        fence->Reset();
        graphicsQueue->Submit(commandBuffer.Get(), fence.Get());
        swapChain->Present();
        fence->Wait();
    }

    PixelFormat swapChainFormat = PixelFormat::max;
    Instance* instance = nullptr;
    Gpu* gpu = nullptr;
    UniqueRef<Device> device;
    Queue* graphicsQueue = nullptr;
    UniqueRef<Surface> surface;
    UniqueRef<SwapChain> swapChain;
    UniqueRef<Buffer> vertexBuffer;
    UniqueRef<BufferView> vertexBufferView;
    std::array<Texture*, backBufferCount> swapChainTextures {};
    std::array<UniqueRef<TextureView>, backBufferCount> swapChainTextureViews;
    UniqueRef<PipelineLayout> pipelineLayout;
    UniqueRef<GraphicsPipeline> pipeline;
    UniqueRef<ShaderModule> vertexShader;
    UniqueRef<ShaderModule> fragmentShader;
    UniqueRef<CommandBuffer> commandBuffer;
    UniqueRef<Fence> fence;
};

int main(int argc, char* argv[])
{
    TriangleApplication application("RHI-Triangle");
    return application.Run(argc, argv);
}
