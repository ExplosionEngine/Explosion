//
// Created by Kindem on 2025/3/16.
//

#include <Editor/Widget/GraphicsSampleWidget.h>
#include <Editor/Widget/moc_GraphicsSampleWidget.cpp> // NOLINT

namespace Editor {
    struct GraphicsWindowSampleVertex {
        Common::FVec3 position;
    };

    struct GraphicsWindowSampleVsUniform {
        Common::FVec3 color;
    };

    GraphicsWidgetDesc GraphicsSampleWidget::GetGraphicsWidgetDesc()
    {
        GraphicsWidgetDesc result;
        result.textureNum = swapChainTextureNum;
        result.formatQualifiers = {RHI::PixelFormat::rgba8Unorm, RHI::PixelFormat::bgra8Unorm};
        result.presentMode = RHI::PresentMode::immediately;
        return result;
    }

    GraphicsSampleWidget::GraphicsSampleWidget(QWidget* inParent)
        : GraphicsWidget(GetGraphicsWidgetDesc(), inParent)
        , imageReadySemaphore(GetDevice().CreateSemaphore())
        , renderFinishedSemaphore(GetDevice().CreateSemaphore())
        , frameFence(GetDevice().CreateFence(true))
    {
        setFixedWidth(1024);
        setFixedHeight(768);

        FetchSwapChainTextures();

        Render::ShaderCompileOptions shaderCompileOptions;
        shaderCompileOptions.includePaths = {"../Shader/Engine"};
        shaderCompileOptions.byteCodeType = GetDevice().GetGpu().GetInstance().GetRHIType() == RHI::RHIType::directX12 ? Render::ShaderByteCodeType::dxil : Render::ShaderByteCodeType::spirv;
        shaderCompileOptions.withDebugInfo = static_cast<bool>(BUILD_CONFIG_DEBUG);

        {
            Render::ShaderCompileInput shaderCompileInput;
            shaderCompileInput.source = Common::FileUtils::ReadTextFile("../Shader/Editor/GraphicsWindowSample.esl");
            shaderCompileInput.stage = RHI::ShaderStageBits::sVertex;
            shaderCompileInput.entryPoint = "VSMain";
            vsCompileOutput = Render::ShaderCompiler::Get().Compile(shaderCompileInput, shaderCompileOptions).get();
        }

        {
            Render::ShaderCompileInput shaderCompileInput;
            shaderCompileInput.source = Common::FileUtils::ReadTextFile("../Shader/Editor/GraphicsWindowSample.esl");
            shaderCompileInput.stage = RHI::ShaderStageBits::sPixel;
            shaderCompileInput.entryPoint = "PSMain";
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
                            RHI::VertexBufferLayout(RHI::VertexStepMode::perVertex, sizeof(GraphicsWindowSampleVertex))
                                .AddAttribute(RHI::VertexAttribute(vsCompileOutput.reflectionData.QueryVertexBindingChecked("POSITION"), RHI::VertexFormat::float32X3, 0))))
                .SetFragmentState(
                    RHI::FragmentState()
                        .AddColorTarget(RHI::ColorTargetState(swapChainTextures[0]->GetCreateInfo().format, RHI::ColorWriteBits::all)))
                .SetPrimitiveState(RHI::PrimitiveState(RHI::PrimitiveTopologyType::triangle, RHI::FillMode::solid, RHI::IndexFormat::uint16, RHI::FrontFace::ccw, RHI::CullMode::none)));

        {
            const std::vector<GraphicsWindowSampleVertex> vertices = {
                {{-.5f, -.5f, 0.f}},
                {{.5f, -.5f, 0.f}},
                {{0.f, .5f, 0.f}},
            };
            const auto bufferSize = vertices.size() * sizeof(GraphicsWindowSampleVertex);

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
                    .SetExtendVertex(sizeof(GraphicsWindowSampleVertex)));
        }

        {
            uniformBuffer = GetDevice().CreateBuffer(
                RHI::BufferCreateInfo()
                    .SetSize(sizeof(GraphicsWindowSampleVsUniform))
                    .SetUsages(RHI::BufferUsageBits::uniform | RHI::BufferUsageBits::mapWrite | RHI::BufferUsageBits::copySrc)
                    .SetInitialState(RHI::BufferState::staging)
                    .SetDebugName("vsUniform"));

            uniformBufferView = uniformBuffer->CreateBufferView(
                RHI::BufferViewCreateInfo()
                    .SetType(RHI::BufferViewType::uniformBinding)
                    .SetSize(sizeof(GraphicsWindowSampleVsUniform))
                    .SetOffset(0));
        }

        bindGroup = GetDevice().CreateBindGroup(
            RHI::BindGroupCreateInfo(bindGroupLayout.Get())
                .AddEntry(RHI::BindGroupEntry(vsCompileOutput.reflectionData.QueryResourceBindingChecked("vsUniform").second, uniformBufferView.Get())));

        commandBuffer = GetDevice().CreateCommandBuffer();
    }

    GraphicsSampleWidget::~GraphicsSampleWidget()
    {
        WaitDeviceIdle();
    }

    void GraphicsSampleWidget::resizeEvent(QResizeEvent* event)
    {
        GraphicsWidget::resizeEvent(event);

        FetchSwapChainTextures();
    }

    void GraphicsSampleWidget::FetchSwapChainTextures()
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

    void GraphicsSampleWidget::OnDrawFrame()
    {
        GraphicsWidget::OnDrawFrame();

        frameFence->Wait();
        frameFence->Reset();

        const double currentTimeSeconds = GetCurrentTimeSeconds();
        const GraphicsWindowSampleVsUniform uniform = {{
            (std::sin(currentTimeSeconds) + 1) / 2,
            (std::cos(currentTimeSeconds) + 1) / 2,
            std::abs(std::sin(currentTimeSeconds))
        }};
        auto* uniformData = uniformBuffer->Map(RHI::MapMode::write, 0, sizeof(GraphicsWindowSampleVsUniform));
        memcpy(uniformData, &uniform, sizeof(GraphicsWindowSampleVsUniform));
        uniformBuffer->UnMap();

        const auto backTextureIndex = GetSwapChain().AcquireBackTexture(imageReadySemaphore.Get());
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
        GetSwapChain().Present(renderFinishedSemaphore.Get());
    }
} // namespace Editor
