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

class TriangleApplication final : public Application {
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
        const auto backTextureIndex = swapChain->AcquireBackTexture(backBufferReadySemaphores[nextFrameIndex].Get());
        inflightFences[nextFrameIndex]->Reset();

        const UniqueRef<CommandRecorder> commandRecorder = commandBuffers[nextFrameIndex]->Begin();
        {
            commandRecorder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::present, TextureState::renderTarget));
            const UniqueRef<RasterPassCommandRecorder> rasterRecorder = commandRecorder->BeginRasterPass(
                RasterPassBeginInfo()
                    .AddColorAttachment(ColorAttachment(swapChainTextureViews[backTextureIndex].Get(), LoadOp::clear, StoreOp::store, LinearColorConsts::black)));
            {
                rasterRecorder->SetPipeline(pipeline.Get());
                rasterRecorder->SetScissor(0, 0, GetWindowWidth(), GetWindowHeight());
                rasterRecorder->SetViewport(0, 0, static_cast<float>(GetWindowWidth()), static_cast<float>(GetWindowHeight()), 0, 1);
                rasterRecorder->SetPrimitiveTopology(PrimitiveTopology::triangleList);
                rasterRecorder->SetVertexBuffer(0, vertexBufferView.Get());
                rasterRecorder->Draw(3, 1, 0, 0);
            }
            rasterRecorder->EndPass();
            commandRecorder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::renderTarget, TextureState::present));
        }
        commandRecorder->End();

        graphicsQueue->Submit(
            commandBuffers[nextFrameIndex].Get(),
            QueueSubmitInfo()
                .AddWaitSemaphore(backBufferReadySemaphores[nextFrameIndex].Get())
                .AddSignalSemaphore(renderFinishedSemaphores[nextFrameIndex].Get())
                .SetSignalFence(inflightFences[nextFrameIndex].Get()));

        swapChain->Present(renderFinishedSemaphores[nextFrameIndex].Get());
        nextFrameIndex = (nextFrameIndex + 1) % backBufferCount;
    }

    void OnDestroy() override
    {
        const UniqueRef<Fence> fence = device->CreateFence(false);
        graphicsQueue->Flush(fence.Get());
        fence->Wait();
    }

private:
    static constexpr uint8_t backBufferCount = 2;

    void SelectGPU()
    {
        gpu = GetRHIInstance()->GetGpu(0);
    }

    void RequestDeviceAndFetchQueues()
    {
        device = gpu->RequestDevice(
            DeviceCreateInfo()
                .AddQueueRequest(QueueRequestInfo(QueueType::graphics, 1)));
        graphicsQueue = device->GetQueue(QueueType::graphics, 0);
    }

    void CreateSwapChain()
    {
        static std::vector swapChainFormatQualifiers = {
            PixelFormat::rgba8Unorm,
            PixelFormat::bgra8Unorm
        };

        surface = device->CreateSurface(SurfaceCreateInfo(GetPlatformWindow()));

        for (const auto format : swapChainFormatQualifiers) {
            if (device->CheckSwapChainFormatSupport(surface.Get(), format)) {
                swapChainFormat = format;
                break;
            }
        }
        Assert(swapChainFormat != PixelFormat::max);

        swapChain = device->CreateSwapChain(
            SwapChainCreateInfo()
                .SetFormat(swapChainFormat)
                .SetPresentMode(PresentMode::immediately)
                .SetTextureNum(backBufferCount)
                .SetWidth(GetWindowWidth())
                .SetHeight(GetWindowHeight())
                .SetSurface(surface.Get())
                .SetPresentQueue(graphicsQueue));

        for (auto i = 0; i < backBufferCount; i++) {
            swapChainTextures[i] = swapChain->GetTexture(i);

            swapChainTextureViews[i] = swapChainTextures[i]->CreateTextureView(
                TextureViewCreateInfo()
                    .SetDimension(TextureViewDimension::tv2D)
                    .SetMipLevels(0, 1)
                    .SetArrayLayers(0, 1)
                    .SetAspect(TextureAspect::color)
                    .SetType(TextureViewType::colorAttachment));
        }
    }

    void CreateVertexBuffer()
    {
        const std::vector<Vertex> vertices = {
            {{-.5f, -.5f, 0.f}, {1.f, 0.f, 0.f}},
            {{.5f, -.5f, 0.f}, {0.f, 1.f, 0.f}},
            {{0.f, .5f, 0.f}, {0.f, 0.f, 1.f}},
        };

        const BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
            .SetSize(vertices.size() * sizeof(Vertex))
            .SetUsages(BufferUsageBits::vertex | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .SetInitialState(BufferState::staging)
            .SetDebugName("vertexBuffer");

        vertexBuffer = device->CreateBuffer(bufferCreateInfo);
        if (vertexBuffer != nullptr) {
            auto* data = vertexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
            memcpy(data, vertices.data(), bufferCreateInfo.size);
            vertexBuffer->UnMap();
        }

        const BufferViewCreateInfo bufferViewCreateInfo = BufferViewCreateInfo()
            .SetType(BufferViewType::vertex)
            .SetSize(vertices.size() * sizeof(Vertex))
            .SetOffset(0)
            .SetExtendVertex(sizeof(Vertex));
        vertexBufferView = vertexBuffer->CreateBufferView(bufferViewCreateInfo);
    }

    void CreatePipelineLayout()
    {
        pipelineLayout = device->CreatePipelineLayout(PipelineLayoutCreateInfo());
    }

    void CreatePipeline()
    {
        vsCompileOutput = CompileShader("../Test/Sample/RHI-Triangle/Triangle.hlsl", "VSMain", RHI::ShaderStageBits::sVertex);
        vertexShader = device->CreateShaderModule(ShaderModuleCreateInfo("VSMain", vsCompileOutput.byteCode));

        psCompileOutput = CompileShader("../Test/Sample/RHI-Triangle/Triangle.hlsl", "PSMain", RHI::ShaderStageBits::sPixel);
        pixelShader = device->CreateShaderModule(ShaderModuleCreateInfo("PSMain", psCompileOutput.byteCode));

        RasterPipelineCreateInfo createInfo = RasterPipelineCreateInfo(pipelineLayout.Get())
            .SetVertexShader(vertexShader.Get())
            .SetPixelShader(pixelShader.Get())
            .SetFragmentState(
                FragmentState()
                    .AddColorTarget(ColorTargetState(swapChainFormat, ColorWriteBits::all)))
            .SetPrimitiveState(PrimitiveState(PrimitiveTopologyType::triangle, FillMode::solid, IndexFormat::uint16, FrontFace::ccw, CullMode::none));

        const auto& vsReflectionData = vsCompileOutput.reflectionData;
        createInfo.SetVertexState(
            VertexState()
                .AddVertexBufferLayout(
                    VertexBufferLayout(VertexStepMode::perVertex, sizeof(Vertex))
                        .AddAttribute(VertexAttribute(vsReflectionData.QueryVertexBindingChecked("POSITION"), VertexFormat::float32X3, 0))
                        .AddAttribute(VertexAttribute(vsReflectionData.QueryVertexBindingChecked("COLOR"), VertexFormat::float32X3, offsetof(Vertex, color)))));

        pipeline = device->CreateRasterPipeline(createInfo);
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
    UniqueRef<RasterPipeline> pipeline;
    UniqueRef<ShaderModule> vertexShader;
    ShaderCompileOutput vsCompileOutput;
    UniqueRef<ShaderModule> pixelShader;
    ShaderCompileOutput psCompileOutput;
    std::array<UniqueRef<CommandBuffer>, backBufferCount> commandBuffers;
    std::array<UniqueRef<Semaphore>, backBufferCount> backBufferReadySemaphores;
    std::array<UniqueRef<Semaphore>, backBufferCount> renderFinishedSemaphores;
    std::array<UniqueRef<Fence>, backBufferCount> inflightFences;
    uint8_t nextFrameIndex = 0;
};

int main(int argc, char* argv[])
{
    TriangleApplication application("RHI-Triangle");
    if (!application.Initialize(argc, argv)) {
        return -1;
    }
    return application.RunLoop();
}
