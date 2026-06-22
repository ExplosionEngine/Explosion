//
// Created by johnk on 2026/6/21.
//

#include <cmath>

#include <QResizeEvent>
#include <QHBoxLayout>

#include <Common/Time.h>
#include <Common/FileSystem.h>
#include <Editor/Widget/Prototype.h>
#include <Editor/Widget/moc_Prototype.cpp> // NOLINT

// In a unity build <windows.h> may be pulled in by an earlier translation unit after RHI's own headers were already
// included (and thus their `#undef CreateSemaphore` skipped via #pragma once), leaving the Win32 `CreateSemaphore`
// macro active. Drop it so the RHI::Device::CreateSemaphore() calls below resolve to the real method.
#undef CreateSemaphore

namespace Editor::Internal {
    constexpr float pi = 3.14159265358979323846f;
    constexpr float twoPi = 2.0f * pi;
    constexpr float degToRad = pi / 180.0f;
    constexpr float radToDeg = 180.0f / pi;
    constexpr float defaultRotationSpeedDegrees = 90.0f;

    struct PrototypeVertex {
        Common::FVec3 position;
    };

    struct PrototypeVsUniform {
        float rotation;
        float padding0;
        float padding1;
        float padding2;
    };
}

namespace Editor {
    PrototypeTriangleWidget::PrototypeTriangleWidget(QWidget* inParent)
        : GraphicsWidget(inParent)
        , rotationSpeed(Internal::defaultRotationSpeedDegrees * Internal::degToRad)
        , rotation(0.0f)
        , lastFrameSeconds(-1.0)
        , imageReadySemaphore(GetDevice().CreateSemaphore())
        , renderFinishedSemaphore(GetDevice().CreateSemaphore())
        , frameFence(GetDevice().CreateFence(true))
        , drawThread(Common::MakeUnique<Common::WorkerThread>("PrototypeDrawThread"))
    {
        setMinimumSize(320, 240);
        resize(640, 480);
        RecreateSwapChain(width(), height());

        Render::ShaderCompileOptions shaderCompileOptions;
        shaderCompileOptions.byteCodeType = GetDevice().GetGpu().GetInstance().GetRHIType() == RHI::RHIType::directX12 ? Render::ShaderByteCodeType::dxil : Render::ShaderByteCodeType::spirv;
        shaderCompileOptions.withDebugInfo = static_cast<bool>(BUILD_CONFIG_DEBUG); // NOLINT

        {
            Render::ShaderCompileInput shaderCompileInput;
            shaderCompileInput.source = Common::FileUtils::ReadTextFile("../Shader/Editor/Prototype.esl").Unwrap();
            shaderCompileInput.stage = RHI::ShaderStageBits::sVertex;
            shaderCompileInput.entryPoint = "VSMain";
            shaderCompileInput.includeDirectories.emplace_back("../Shader/Explosion");
            vsCompileOutput = Render::ShaderCompiler::Get().Compile(shaderCompileInput, shaderCompileOptions).get();
        }

        {
            Render::ShaderCompileInput shaderCompileInput;
            shaderCompileInput.source = Common::FileUtils::ReadTextFile("../Shader/Editor/Prototype.esl").Unwrap();
            shaderCompileInput.stage = RHI::ShaderStageBits::sPixel;
            shaderCompileInput.entryPoint = "PSMain";
            shaderCompileInput.includeDirectories.emplace_back("../Shader/Explosion");
            psCompileOutput = Render::ShaderCompiler::Get().Compile(shaderCompileInput, shaderCompileOptions).get();
        }

        vsModule = GetDevice().CreateShaderModule(RHI::ShaderModuleCreateInfo("VSMain", vsCompileOutput.byteCode));
        psModule = GetDevice().CreateShaderModule(RHI::ShaderModuleCreateInfo("PSMain", psCompileOutput.byteCode));

        bindGroupLayout = GetDevice().CreateBindGroupLayout(
            RHI::BindGroupLayoutCreateInfo(0)
                .AddEntry(RHI::BindGroupLayoutEntry(vsCompileOutput.reflectionData.QueryResourceBindingChecked("vsUniform").second, RHI::ShaderStageBits::sVertex)));

        pipelineLayout = GetDevice().CreatePipelineLayout(
            RHI::PipelineLayoutCreateInfo()
                .AddBindGroupLayout(bindGroupLayout.Get()));

        pipeline = GetDevice().CreateRasterPipeline(
            RHI::RasterPipelineCreateInfo(pipelineLayout.Get())
                .SetVertexShader(vsModule.Get())
                .SetPixelShader(psModule.Get())
                .SetVertexState(
                    RHI::VertexState()
                        .AddVertexBufferLayout(
                            RHI::VertexBufferLayout(RHI::VertexStepMode::perVertex, sizeof(Internal::PrototypeVertex))
                                .AddAttribute(RHI::VertexAttribute(vsCompileOutput.reflectionData.QueryVertexBindingChecked("POSITION"), RHI::VertexFormat::float32X3, 0))))
                .SetFragmentState(
                    RHI::FragmentState()
                        .AddColorTarget(RHI::ColorTargetState(swapChainTextures[0]->GetCreateInfo().format, RHI::ColorWriteBits::all)))
                .SetPrimitiveState(RHI::PrimitiveState(RHI::PrimitiveTopologyType::triangle, RHI::FillMode::solid, RHI::IndexFormat::uint16, RHI::FrontFace::ccw, RHI::CullMode::none)));

        {
            const std::vector<Internal::PrototypeVertex> vertices = {
                {{-.5f, -.5f, 0.f}},
                {{.5f, -.5f, 0.f}},
                {{0.f, .5f, 0.f}},
            };
            const auto bufferSize = vertices.size() * sizeof(Internal::PrototypeVertex);

            vertexBuffer = GetDevice().CreateBuffer(
                RHI::BufferCreateInfo()
                    .SetSize(bufferSize)
                    .SetUsages(RHI::BufferUsageBits::vertex | RHI::BufferUsageBits::mapWrite | RHI::BufferUsageBits::copySrc)
                    .SetInitialState(RHI::BufferState::staging)
                    .SetDebugName("vertexBuffer"));

            auto* data = vertexBuffer->Map(RHI::MapMode::write, 0, bufferSize);
            memcpy(data, vertices.data(), bufferSize);
            vertexBuffer->UnMap();

            vertexBufferView = vertexBuffer->CreateBufferView(
                RHI::BufferViewCreateInfo()
                    .SetType(RHI::BufferViewType::vertex)
                    .SetSize(bufferSize)
                    .SetOffset(0)
                    .SetExtendVertex(sizeof(Internal::PrototypeVertex)));
        }

        {
            uniformBuffer = GetDevice().CreateBuffer(
                RHI::BufferCreateInfo()
                    .SetSize(sizeof(Internal::PrototypeVsUniform))
                    .SetUsages(RHI::BufferUsageBits::uniform | RHI::BufferUsageBits::mapWrite | RHI::BufferUsageBits::copySrc)
                    .SetInitialState(RHI::BufferState::staging)
                    .SetDebugName("vsUniform"));

            uniformBufferView = uniformBuffer->CreateBufferView(
                RHI::BufferViewCreateInfo()
                    .SetType(RHI::BufferViewType::uniformBinding)
                    .SetSize(sizeof(Internal::PrototypeVsUniform))
                    .SetOffset(0));
        }

        bindGroup = GetDevice().CreateBindGroup(
            RHI::BindGroupCreateInfo(bindGroupLayout.Get())
                .AddEntry(RHI::BindGroupEntry(vsCompileOutput.reflectionData.QueryResourceBindingChecked("vsUniform").second, uniformBufferView.Get())));

        commandBuffer = GetDevice().CreateCommandBuffer();

        running = true;
        DispatchFrame();
    }

    PrototypeTriangleWidget::~PrototypeTriangleWidget()
    {
        running = false;
        drawThread.Reset();
        WaitDeviceIdle();
    }

    void PrototypeTriangleWidget::SetRotationSpeed(float inRadiansPerSecond)
    {
        rotationSpeed.store(inRadiansPerSecond);
    }

    float PrototypeTriangleWidget::GetRotationSpeed() const
    {
        return rotationSpeed.load();
    }

    void PrototypeTriangleWidget::resizeEvent(QResizeEvent* event)
    {
        GraphicsWidget::resizeEvent(event);

        drawThread->EmplaceTask([this, size = event->size()]() -> void {
            RecreateSwapChain(size.width(), size.height());
        });
    }

    void PrototypeTriangleWidget::RecreateSwapChain(uint32_t inWidth, uint32_t inHeight)
    {
        static std::vector<RHI::PixelFormat> formatQualifiers = {
            RHI::PixelFormat::rgba8Unorm,
            RHI::PixelFormat::bgra8Unorm
        };

        if (swapChain != nullptr) {
            WaitDeviceIdle();
            swapChain.Reset();
        }

        std::optional<RHI::PixelFormat> pixelFormat = {};
        for (const auto format : formatQualifiers) {
            if (device->CheckSwapChainFormatSupport(surface.Get(), format)) {
                pixelFormat = format;
                break;
            }
        }
        Assert(pixelFormat.has_value());

        swapChain = device->CreateSwapChain(
            RHI::SwapChainCreateInfo()
                .SetPresentQueue(device->GetQueue(RHI::QueueType::graphics, 0))
                .SetSurface(surface.Get())
                .SetTextureNum(2)
                .SetFormat(pixelFormat.value())
                .SetWidth(inWidth)
                .SetHeight(inHeight)
                .SetPresentMode(RHI::PresentMode::immediately));

        for (auto i = 0; i < swapChainTextureNum; i++) {
            swapChainTextures[i] = swapChain->GetTexture(i);
            swapChainTextureViews[i] = swapChainTextures[i]->CreateTextureView(
                RHI::TextureViewCreateInfo()
                    .SetDimension(RHI::TextureViewDimension::tv2D)
                    .SetMipLevels(0, 1)
                    .SetArrayLayers(0, 1)
                    .SetAspect(RHI::TextureAspect::color)
                    .SetType(RHI::TextureViewType::colorAttachment));
        }
    }

    void PrototypeTriangleWidget::DispatchFrame()
    {
        if (!running) {
            return;
        }
        drawThread->EmplaceTask([this]() -> void { DrawFrame(); });
    }

    void PrototypeTriangleWidget::DrawFrame()
    {
        frameFence->Wait();
        frameFence->Reset();

        const double currentTimeSeconds = Common::TimePoint::Now().ToSeconds();
        if (lastFrameSeconds >= 0.0) {
            rotation += static_cast<float>(currentTimeSeconds - lastFrameSeconds) * rotationSpeed.load();
            rotation = std::fmod(rotation, Internal::twoPi);
        }
        lastFrameSeconds = currentTimeSeconds;

        const Internal::PrototypeVsUniform uniform { rotation, 0.0f, 0.0f, 0.0f };
        auto* uniformData = uniformBuffer->Map(RHI::MapMode::write, 0, sizeof(Internal::PrototypeVsUniform));
        memcpy(uniformData, &uniform, sizeof(Internal::PrototypeVsUniform));
        uniformBuffer->UnMap();

        const auto backTextureIndex = swapChain->AcquireBackTexture(imageReadySemaphore.Get());
        const Common::UniquePtr<RHI::CommandRecorder> commandRecorder = commandBuffer->Begin();
        {
            commandRecorder->ResourceBarrier(RHI::Barrier::Transition(swapChainTextures[backTextureIndex], RHI::TextureState::present, RHI::TextureState::renderTarget));
            const Common::UniquePtr<RHI::RasterPassCommandRecorder> rasterRecorder = commandRecorder->BeginRasterPass(
                RHI::RasterPassBeginInfo()
                    .AddColorAttachment(RHI::ColorAttachment(swapChainTextureViews[backTextureIndex].Get(), RHI::LoadOp::clear, RHI::StoreOp::store, Common::LinearColorConsts::black)));
            {
                rasterRecorder->SetPipeline(pipeline.Get());
                rasterRecorder->SetBindGroup(0, bindGroup.Get());
                rasterRecorder->SetScissor(0, 0, width(), height());
                rasterRecorder->SetViewport(0, 0, static_cast<float>(width()), static_cast<float>(height()), 0, 1);
                rasterRecorder->SetPrimitiveTopology(RHI::PrimitiveTopology::triangleList);
                rasterRecorder->SetVertexBuffer(0, vertexBufferView.Get());
                rasterRecorder->Draw(3, 1, 0, 0);
            }
            rasterRecorder->EndPass();
            commandRecorder->ResourceBarrier(RHI::Barrier::Transition(swapChainTextures[backTextureIndex], RHI::TextureState::renderTarget, RHI::TextureState::present));
        }
        commandRecorder->End();

        GetDevice().GetQueue(RHI::QueueType::graphics, 0)->Submit(
            commandBuffer.Get(),
            RHI::QueueSubmitInfo()
                .AddWaitSemaphore(imageReadySemaphore.Get())
                .AddSignalSemaphore(renderFinishedSemaphore.Get())
                .SetSignalFence(frameFence.Get()));
        swapChain->Present(renderFinishedSemaphore.Get());

        DispatchFrame();
    }

    PrototypeBackend::PrototypeBackend(PrototypeTriangleWidget* inTriangle, QObject* inParent)
        : QObject(inParent)
        , triangle(inTriangle)
    {
    }

    PrototypeBackend::~PrototypeBackend() = default;

    void PrototypeBackend::SetRotationSpeed(double inDegreesPerSecond)
    {
        triangle->SetRotationSpeed(static_cast<float>(inDegreesPerSecond) * Internal::degToRad);
    }

    double PrototypeBackend::GetRotationSpeed() const
    {
        return static_cast<double>(triangle->GetRotationSpeed() * Internal::radToDeg);
    }

    PrototypeWebWidget::PrototypeWebWidget(PrototypeTriangleWidget* inTriangle, QWidget* inParent)
        : WebWidget(inParent)
    {
        backend = new PrototypeBackend(inTriangle, this);
        GetWebChannel()->registerObject("backend", backend);
        Load("/prototype");
    }

    PrototypeWebWidget::~PrototypeWebWidget() = default;

    PrototypePlayground::PrototypePlayground(QWidget* inParent)
        : QWidget(inParent)
    {
        setWindowTitle("Explosion Prototype Playground");
        resize(1280, 720);

        triangle = new PrototypeTriangleWidget(this);
        web = new PrototypeWebWidget(triangle, this);

        auto* layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->addWidget(triangle, 1);
        layout->addWidget(web, 1);
    }

    PrototypePlayground::~PrototypePlayground() = default;
} // namespace Editor
