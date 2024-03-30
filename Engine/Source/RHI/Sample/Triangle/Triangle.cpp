//
// Created by johnk on 9/1/2022.
//

#include <vector>
#include <array>

#include <Application.h>
#include <Common/Math/Vector.h>
using namespace RHI;

struct Vertex {
    FVec3 position;
    FVec3 color;
};

class TriangleApplication : public Application {
public:
    NonCopyable(TriangleApplication)
    explicit TriangleApplication(const std::string& n) : Application(n) {}
    ~TriangleApplication() override = default;

protected:
    void OnCreate() override
    {
        SelectGPU();
        RequestDeviceAndFetchQueues();
        CreateSwapChain();

        CreatePipelineLayout();
        CreatePipeline();
        CreateVertexBuffer();

        CreateSyncObjects();
        CreateCommandBuffer();
    }

    void OnDrawFrame() override
    {
        inflightFences[nextFrameIndex]->Wait();
        auto backTextureIndex = swapChain->AcquireBackTexture(backBufferReadySemaphores[nextFrameIndex].Get());
        inflightFences[nextFrameIndex]->Reset();

        UniqueRef<CommandEncoder> commandEncoder = commandBuffers[nextFrameIndex]->Begin();
        {
            commandEncoder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::present, TextureState::renderTarget));
            UniqueRef<GraphicsPassCommandEncoder> graphicsEncoder = commandEncoder->BeginGraphicsPass(
                GraphicsPassBeginInfo()
                    .ColorAttachment(
                        GraphicsPassColorAttachment()
                            .ClearValue(Common::ColorConsts::black.ToLinearColor())
                            .LoadOp(LoadOp::clear)
                            .StoreOp(StoreOp::store)
                            .View(swapChainTextureViews[backTextureIndex].Get())));
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
        commandEncoder->End();

        graphicsQueue->Submit(
            commandBuffers[nextFrameIndex].Get(),
            QueueSubmitInfo()
                .WaitSemaphore(backBufferReadySemaphores[nextFrameIndex].Get())
                .SignalSemaphore(renderFinishedSemaphores[nextFrameIndex].Get())
                .SignalFence(inflightFences[nextFrameIndex].Get()));

        swapChain->Present(renderFinishedSemaphores[nextFrameIndex].Get());
        nextFrameIndex = (nextFrameIndex + 1) % backBufferCount;
    }

    void OnDestroy() override
    {
        Common::UniqueRef<Fence> fence = device->CreateFence(false);
        graphicsQueue->Flush(fence.Get());
        fence->Wait();
    }

private:
    static constexpr uint8_t backBufferCount = 2;

    void SelectGPU()
    {
        gpu = instance->GetGpu(0);
    }

    void RequestDeviceAndFetchQueues()
    {
        device = gpu->RequestDevice(
            DeviceCreateInfo()
                .Queue(QueueRequestInfo(QueueType::graphics, 1)));
        graphicsQueue = device->GetQueue(QueueType::graphics, 0);
    }

    void CreateSwapChain()
    {
        static std::vector<PixelFormat> swapChainFormatQualifiers = {
            PixelFormat::rgba8Unorm,
            PixelFormat::bgra8Unorm
        };

        surface = device->CreateSurface(
            SurfaceCreateInfo()
                .Window(GetPlatformWindow()));

        for (auto format : swapChainFormatQualifiers) {
            if (device->CheckSwapChainFormatSupport(surface.Get(), format)) {
                swapChainFormat = format;
                break;
            }
        }
        Assert(swapChainFormat != PixelFormat::max);

        swapChain = device->CreateSwapChain(
            SwapChainCreateInfo()
                .Format(swapChainFormat)
                .PresentMode(PresentMode::immediately)
                .TextureNum(backBufferCount)
                .Extent({ width, height })
                .Surface(surface.Get())
                .PresentQueue(graphicsQueue));

        for (auto i = 0; i < backBufferCount; i++) {
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

        BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
            .Size(vertices.size() * sizeof(Vertex))
            .Usages(BufferUsageBits::vertex | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .InitialState(BufferState::staging)
            .DebugName("vertexBuffer");

        vertexBuffer = device->CreateBuffer(bufferCreateInfo);
        if (vertexBuffer != nullptr) {
            auto* data = vertexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
            memcpy(data, vertices.data(), bufferCreateInfo.size);
            vertexBuffer->UnMap();
        }

        BufferViewCreateInfo bufferViewCreateInfo = BufferViewCreateInfo()
            .Type(BufferViewType::vertex)
            .Size(vertices.size() * sizeof(Vertex))
            .Offset(0)
            .ExtendVertex(sizeof(Vertex));
        vertexBufferView = vertexBuffer->CreateBufferView(bufferViewCreateInfo);
    }

    void CreatePipelineLayout()
    {
        pipelineLayout = device->CreatePipelineLayout(PipelineLayoutCreateInfo());
    }

    void CreatePipeline()
    {
        std::vector<uint8_t> vsByteCode;
        CompileShader(vsByteCode, "../Test/Sample/Triangle/Triangle.hlsl", "VSMain", RHI::ShaderStageBits::sVertex);

        ShaderModuleCreateInfo shaderModuleCreateInfo {};
        shaderModuleCreateInfo.size = vsByteCode.size();
        shaderModuleCreateInfo.byteCode = vsByteCode.data();
        vertexShader = device->CreateShaderModule(shaderModuleCreateInfo);

        std::vector<uint8_t> fsByteCode;
        CompileShader(fsByteCode, "../Test/Sample/Triangle/Triangle.hlsl", "FSMain", RHI::ShaderStageBits::sPixel);

        shaderModuleCreateInfo.size = fsByteCode.size();
        shaderModuleCreateInfo.byteCode = fsByteCode.data();
        fragmentShader = device->CreateShaderModule(shaderModuleCreateInfo);

        GraphicsPipelineCreateInfo createInfo = GraphicsPipelineCreateInfo()
            .Layout(pipelineLayout.Get())
            .VertexShader(vertexShader.Get())
            .PixelShader(fragmentShader.Get())
            .VertexState(
                VertexState()
                    .VertexBufferLayout(
                        VertexBufferLayout()
                            .StepMode(VertexStepMode::perVertex)
                            .Stride(sizeof(Vertex))
                            .Attribute(
                                VertexAttribute()
                                    .Format(RHI::VertexFormat::float32X3)
                                    .Offset(0)
                                    .SemanticName("POSITION")
                                    .SemanticIndex(0))
                            .Attribute(
                                VertexAttribute()
                                    .Format(RHI::VertexFormat::float32X3)
                                    .Offset(offsetof(Vertex, color))
                                    .SemanticName("COLOR")
                                    .SemanticIndex(0))))
            .FragmentState(
                FragmentState()
                    .ColorTarget(
                        ColorTargetState()
                            .Format(swapChainFormat)
                            .WriteFlags(ColorWriteBits::red | ColorWriteBits::green | ColorWriteBits::blue | ColorWriteBits::alpha)))
            .PrimitiveState(
                PrimitiveState()
                    .DepthClip(false)
                    .FrontFace(FrontFace::ccw)
                    .CullMode(CullMode::none)
                    .TopologyType(PrimitiveTopologyType::triangle)
                    .StripIndexFormat(IndexFormat::uint16))
            .DepthStencilState(
                DepthStencilState()
                    .DepthEnabled(false)
                    .StencilEnabled(false))
            .MultiSampleState(
                MultiSampleState()
                    .Count(1));

        pipeline = device->CreateGraphicsPipeline(createInfo);
    }

    void CreateSyncObjects()
    {
        for (auto i = 0; i < backBufferCount; i++) {
            backBufferReadySemaphores[i] = device->CreateSemaphore();
            renderFinishedSemaphores[i] = device->CreateSemaphore();
            inflightFences[i] = device->CreateFence(true);
        }
    }

    void CreateCommandBuffer()
    {
        for (auto i = 0; i < backBufferCount; i++) {
            commandBuffers[i] = device->CreateCommandBuffer();
        }
    }

    PixelFormat swapChainFormat = PixelFormat::max;
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
    std::array<UniqueRef<CommandBuffer>, backBufferCount> commandBuffers;
    std::array<UniqueRef<Semaphore>, backBufferCount> backBufferReadySemaphores;
    std::array<UniqueRef<Semaphore>, backBufferCount> renderFinishedSemaphores;
    std::array<UniqueRef<Fence>, backBufferCount> inflightFences;
    uint8_t nextFrameIndex = 0;
};

int main(int argc, char* argv[])
{
    TriangleApplication application("RHI-Triangle");
    return application.Run(argc, argv);
}
