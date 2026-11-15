#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <imgui.h>

#include <Common/File.h>
#include <Common/Math/Color.h>
#include <Editor/ImGui/ImGuiDrawData.h>
#include <Editor/ImGui/ImGuiRhiRenderer.h>

namespace Editor::ImGuiRendering::Internal {
    const Common::LinearColor uiOnlyClearColor = Common::LinearColor(0.08f, 0.08f, 0.09f, 1.0f);
    constexpr int imguiVertexBufferInitialCapacity = 5000;
    constexpr int imguiIndexBufferInitialCapacity = 10000;
    constexpr RHI::IndexFormat imguiIndexFormat = sizeof(ImDrawIdx) == 2
        ? RHI::IndexFormat::uint16
        : RHI::IndexFormat::uint32;

    static Render::ShaderCompileOutput CompileImGuiShader(
        RHI::Device& inDevice,
        const std::string& inEntryPoint,
        RHI::ShaderStageBits inStage)
    {
        Render::ShaderCompileInput input;
        input.source = Common::FileUtils::ReadTextFile("../Shader/Editor/ImGui.esl").Unwrap();
        input.entryPoint = inEntryPoint;
        input.stage = inStage;
        input.includeDirectories.emplace_back("../Shader/Explosion");

        Render::ShaderCompileOptions options;
        options.byteCodeType = inDevice.GetGpu().GetInstance().GetRHIType() == RHI::RHIType::directX12
            ? Render::ShaderByteCodeType::dxil
            : Render::ShaderByteCodeType::spirv;
        options.withDebugInfo = static_cast<bool>(BUILD_CONFIG_DEBUG); // NOLINT

        auto future = Render::ShaderCompiler::Get().Compile(input, options);
        future.wait();
        return future.get();
    }
}

namespace Editor {
    ImGuiRhiRenderer::ImGuiRhiRenderer(RHI::Device& inDevice, Render::RenderSurface& inSurface, Render::RenderThread& inRenderThread)
        : device(inDevice)
        , surface(inSurface)
        , renderThread(inRenderThread)
        , pipelineFormat(surface.GetFormat())
        , uiFrameFence(device.CreateFence(true))
        , uiCommandBuffer(device.CreateCommandBuffer(RHI::QueueType::graphics))
        , imguiVertexBufferCapacity(0)
        , imguiIndexBufferCapacity(0)
    {
        ImGui::GetIO().BackendRendererName = "ExplosionRHI";
        CreateImGuiDeviceObjects();
        CreateImGuiDynamicBuffers(ImGuiRendering::Internal::imguiVertexBufferInitialCapacity, ImGuiRendering::Internal::imguiIndexBufferInitialCapacity);
    }

    ImGuiRhiRenderer::~ImGuiRhiRenderer()
    {
        WaitIdle();
        inFlightDrawData.Reset();
        imguiFrameTextureBindGroups.clear();
        ImGui::GetIO().Fonts->SetTexID(ImTextureID_Invalid);
        ImGui::GetIO().BackendRendererName = nullptr;
    }

    void ImGuiRhiRenderer::NewFrame()
    {
        if (pipelineFormat != surface.GetFormat()) {
            WaitIdle();
            pipelineFormat = surface.GetFormat();
            CreateImGuiDeviceObjects();
        }
    }

    void ImGuiRhiRenderer::Submit(const ImDrawData& inDrawData)
    {
        auto drawData = Common::MakeShared<ImGuiDrawData>(inDrawData);
        renderThread.EmplaceTask([this, drawData]() -> void {
            RenderFrame(drawData);
        });
    }

    void ImGuiRhiRenderer::WaitIdle() const
    {
        renderThread.Flush();
        WaitForPreviousFrame();
    }

    void ImGuiRhiRenderer::RenderFrame(const Common::SharedPtr<ImGuiDrawData>& inDrawData)
    {
        WaitForPreviousFrame();
        inFlightDrawData = inDrawData;
        uiFrameFence->Reset();

        const auto target = surface.GetRenderTarget();
        RenderUiPass(inFlightDrawData->Get(), target);
        RHI::QueueSubmitInfo submitInfo;
        submitInfo.SetSignalFence(uiFrameFence.Get());
        if (target.waitSemaphore != nullptr) {
            submitInfo.AddWaitSemaphore(target.waitSemaphore);
        }
        if (target.signalSemaphore != nullptr) {
            submitInfo.AddSignalSemaphore(target.signalSemaphore);
        }
        device.GetQueue(RHI::QueueType::graphics, 0)->Submit(uiCommandBuffer.Get(), submitInfo);
    }

    void ImGuiRhiRenderer::WaitForPreviousFrame() const
    {
        uiFrameFence->Wait();
    }

    void ImGuiRhiRenderer::RenderUiPass(ImDrawData& inDrawData, const Render::SurfaceTarget& inTarget)
    {
        const auto recorder = uiCommandBuffer->Begin();
        recorder->ResourceBarrier(RHI::Barrier::Transition(inTarget.texture, inTarget.beforeRenderState, RHI::TextureState::renderTarget));
        const auto rasterRecorder = recorder->BeginRasterPass(
            RHI::RasterPassBeginInfo()
                .AddColorAttachment(RHI::ColorAttachment(inTarget.renderTargetView, RHI::LoadOp::clear, RHI::StoreOp::store, ImGuiRendering::Internal::uiOnlyClearColor)));
        RenderImGuiDrawData(inDrawData, *rasterRecorder);
        rasterRecorder->EndPass();
        recorder->ResourceBarrier(RHI::Barrier::Transition(inTarget.texture, RHI::TextureState::renderTarget, inTarget.afterRenderState));
        recorder->End();
    }

    void ImGuiRhiRenderer::RenderImGuiDrawData(ImDrawData& inDrawData, RHI::RasterPassCommandRecorder& inRecorder)
    {
        imguiFrameTextureBindGroups.clear();
        if (inDrawData.TotalVtxCount <= 0 || inDrawData.TotalIdxCount <= 0) {
            return;
        }

        if (inDrawData.TotalVtxCount > imguiVertexBufferCapacity || inDrawData.TotalIdxCount > imguiIndexBufferCapacity) {
            CreateImGuiDynamicBuffers(
                std::max(inDrawData.TotalVtxCount, imguiVertexBufferCapacity * 2),
                std::max(inDrawData.TotalIdxCount, imguiIndexBufferCapacity * 2));
        }

        UpdateImGuiBuffers(inDrawData);
        UpdateImGuiPassParams(inDrawData);

        const auto framebufferWidth = static_cast<uint32_t>(inDrawData.DisplaySize.x * inDrawData.FramebufferScale.x);
        const auto framebufferHeight = static_cast<uint32_t>(inDrawData.DisplaySize.y * inDrawData.FramebufferScale.y);
        if (framebufferWidth == 0 || framebufferHeight == 0) {
            return;
        }

        inRecorder.SetPipeline(imguiPipeline.Get());
        inRecorder.SetVertexBuffer(0, imguiVertexBufferView.Get());
        inRecorder.SetIndexBuffer(imguiIndexBufferView.Get());
        inRecorder.SetPrimitiveTopology(RHI::PrimitiveTopology::triangleList);
        inRecorder.SetViewport(0.0f, 0.0f, static_cast<float>(framebufferWidth), static_cast<float>(framebufferHeight), 0.0f, 1.0f);

        const ImVec2 clipOffset = inDrawData.DisplayPos;
        const ImVec2 clipScale = inDrawData.FramebufferScale;
        int globalVertexOffset = 0;
        int globalIndexOffset = 0;
        for (int commandListIndex = 0; commandListIndex < inDrawData.CmdListsCount; commandListIndex++) {
            const ImDrawList* commandList = inDrawData.CmdLists[commandListIndex];
            for (int commandIndex = 0; commandIndex < commandList->CmdBuffer.Size; commandIndex++) {
                const ImDrawCmd* command = &commandList->CmdBuffer[commandIndex];
                if (command->UserCallback != nullptr) {
                    command->UserCallback(commandList, command);
                    continue;
                }

                ImVec2 clipMin(
                    (command->ClipRect.x - clipOffset.x) * clipScale.x,
                    (command->ClipRect.y - clipOffset.y) * clipScale.y);
                ImVec2 clipMax(
                    (command->ClipRect.z - clipOffset.x) * clipScale.x,
                    (command->ClipRect.w - clipOffset.y) * clipScale.y);
                clipMin.x = std::clamp(clipMin.x, 0.0f, static_cast<float>(framebufferWidth));
                clipMin.y = std::clamp(clipMin.y, 0.0f, static_cast<float>(framebufferHeight));
                clipMax.x = std::clamp(clipMax.x, 0.0f, static_cast<float>(framebufferWidth));
                clipMax.y = std::clamp(clipMax.y, 0.0f, static_cast<float>(framebufferHeight));
                if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y) {
                    continue;
                }

                inRecorder.SetScissor(
                    static_cast<uint32_t>(clipMin.x),
                    static_cast<uint32_t>(clipMin.y),
                    static_cast<uint32_t>(clipMax.x),
                    static_cast<uint32_t>(clipMax.y));
                ImTextureID textureId = command->GetTexID();
                auto* textureView = textureId == ImTextureID_Invalid
                    ? imguiFontTextureView.Get()
                    : reinterpret_cast<RHI::TextureView*>(static_cast<uintptr_t>(textureId));
                Assert(textureView != nullptr);
                inRecorder.SetBindGroup(0, GetOrCreateImGuiTextureBindGroup(*textureView));
                inRecorder.DrawIndexed(
                    command->ElemCount,
                    1,
                    command->IdxOffset + globalIndexOffset,
                    command->VtxOffset + globalVertexOffset,
                    0);
            }
            globalIndexOffset += commandList->IdxBuffer.Size;
            globalVertexOffset += commandList->VtxBuffer.Size;
        }
    }

    void ImGuiRhiRenderer::CreateImGuiDeviceObjects()
    {
        imguiFrameTextureBindGroups.clear();
        imguiVertexShaderCompileOutput = ImGuiRendering::Internal::CompileImGuiShader(device, "VSMain", RHI::ShaderStageBits::sVertex);
        imguiPixelShaderCompileOutput = ImGuiRendering::Internal::CompileImGuiShader(device, "PSMain", RHI::ShaderStageBits::sPixel);
        imguiVertexShader = device.CreateShaderModule(RHI::ShaderModuleCreateInfo("VSMain", imguiVertexShaderCompileOutput.byteCode));
        imguiPixelShader = device.CreateShaderModule(RHI::ShaderModuleCreateInfo("PSMain", imguiPixelShaderCompileOutput.byteCode));

        CreateImGuiFontTexture();

        imguiPassParamsBuffer = device.CreateBuffer(
            RHI::BufferCreateInfo()
                .SetSize(sizeof(Common::Internal::ImGuiPassParams))
                .SetUsages(RHI::BufferUsageBits::uniform | RHI::BufferUsageBits::mapWrite)
                .SetInitialState(RHI::BufferState::staging)
                .SetDebugName("imguiPassParams"));
        imguiPassParamsBufferView = imguiPassParamsBuffer->CreateBufferView(
            RHI::BufferViewCreateInfo()
                .SetType(RHI::BufferViewType::uniformBinding)
                .SetSizeInBytes(sizeof(Common::Internal::ImGuiPassParams))
                .SetOffsetInBytes(0));

        imguiBindGroupLayout = device.CreateBindGroupLayout(
            RHI::BindGroupLayoutCreateInfo(0, "imguiBindGroupLayout")
                .AddEntry(RHI::BindGroupLayoutEntry(imguiPixelShaderCompileOutput.reflectionData.QueryResourceBindingChecked("imageTex").second, RHI::ShaderStageBits::sPixel))
                .AddEntry(RHI::BindGroupLayoutEntry(imguiPixelShaderCompileOutput.reflectionData.QueryResourceBindingChecked("imageSampler").second, RHI::ShaderStageBits::sPixel))
                .AddEntry(RHI::BindGroupLayoutEntry(imguiVertexShaderCompileOutput.reflectionData.QueryResourceBindingChecked("passParams").second, RHI::ShaderStageBits::sVertex)));
        imguiPipelineLayout = device.CreatePipelineLayout(
            RHI::PipelineLayoutCreateInfo()
                .AddBindGroupLayout(imguiBindGroupLayout.Get()));

        const RHI::BlendComponent colorBlend(RHI::BlendOp::opAdd, RHI::BlendFactor::srcAlpha, RHI::BlendFactor::oneMinusSrcAlpha);
        const RHI::BlendComponent alphaBlend(RHI::BlendOp::opAdd, RHI::BlendFactor::one, RHI::BlendFactor::oneMinusSrcAlpha);
        const RHI::ColorTargetState colorTarget(surface.GetFormat(), RHI::ColorWriteBits::all, true, colorBlend, alphaBlend);

        imguiPipeline = device.CreateRasterPipeline(
            RHI::RasterPipelineCreateInfo(imguiPipelineLayout.Get())
                .SetVertexShader(imguiVertexShader.Get())
                .SetPixelShader(imguiPixelShader.Get())
                .SetVertexState(
                    RHI::VertexState()
                        .AddVertexBufferLayout(
                            RHI::VertexBufferLayout(RHI::VertexStepMode::perVertex, sizeof(ImDrawVert))
                                .AddAttribute(RHI::VertexAttribute(imguiVertexShaderCompileOutput.reflectionData.QueryVertexBindingChecked("POSITION"), RHI::VertexFormat::float32X2, offsetof(ImDrawVert, pos)))
                                .AddAttribute(RHI::VertexAttribute(imguiVertexShaderCompileOutput.reflectionData.QueryVertexBindingChecked("TEXCOORD"), RHI::VertexFormat::float32X2, offsetof(ImDrawVert, uv)))
                                .AddAttribute(RHI::VertexAttribute(imguiVertexShaderCompileOutput.reflectionData.QueryVertexBindingChecked("COLOR"), RHI::VertexFormat::unorm8X4, offsetof(ImDrawVert, col)))))
                .SetFragmentState(RHI::FragmentState().AddColorTarget(colorTarget))
                .SetPrimitiveState(RHI::PrimitiveState(RHI::PrimitiveTopologyType::triangle, RHI::FillMode::solid, ImGuiRendering::Internal::imguiIndexFormat, RHI::FrontFace::ccw, RHI::CullMode::none))
                .SetDepthStencilState(RHI::DepthStencilState()));
    }

    void ImGuiRhiRenderer::CreateImGuiFontTexture()
    {

        unsigned char* pixels = nullptr;
        int width = 0;
        int height = 0;
        ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        imguiFontTexture = device.CreateTexture(
            RHI::TextureCreateInfo()
                .SetType(RHI::TextureType::t2D)
                .SetWidth(static_cast<uint32_t>(width))
                .SetHeight(static_cast<uint32_t>(height))
                .SetDepthOrArraySize(1)
                .SetFormat(RHI::PixelFormat::rgba8Unorm)
                .SetUsages(RHI::TextureUsageBits::copyDst | RHI::TextureUsageBits::textureBinding)
                .SetMipLevels(1)
                .SetSamples(1)
                .SetInitialState(RHI::TextureState::undefined)
                .SetDebugName("imguiFontTexture"));
        imguiFontTextureView = imguiFontTexture->CreateTextureView(
            RHI::TextureViewCreateInfo()
                .SetDimension(RHI::TextureViewDimension::tv2D)
                .SetMipLevels(0, 1)
                .SetArrayLayers(0, 1)
                .SetAspect(RHI::TextureAspect::color)
                .SetType(RHI::TextureViewType::textureBinding));
        imguiImageSampler = device.CreateSampler(
            RHI::SamplerCreateInfo()
                .SetAddressModeU(RHI::AddressMode::clampToEdge)
                .SetAddressModeV(RHI::AddressMode::clampToEdge)
                .SetAddressModeW(RHI::AddressMode::clampToEdge)
                .SetMagFilter(RHI::FilterMode::linear)
                .SetMinFilter(RHI::FilterMode::linear)
                .SetMipFilter(RHI::FilterMode::linear)
                .SetDebugName("imguiImageSampler"));

        const auto copyFootprint = device.GetTextureSubResourceCopyFootprint(*imguiFontTexture, RHI::TextureSubResourceInfo());
        auto stagingBuffer = device.CreateBuffer(
            RHI::BufferCreateInfo()
                .SetSize(static_cast<uint32_t>(copyFootprint.totalBytes))
                .SetUsages(RHI::BufferUsageBits::mapWrite | RHI::BufferUsageBits::copySrc)
                .SetInitialState(RHI::BufferState::staging)
                .SetDebugName("imguiFontUpload"));
        auto* mapped = stagingBuffer->Map(RHI::MapMode::write, 0, copyFootprint.totalBytes);
        const size_t srcRowPitch = static_cast<size_t>(width) * copyFootprint.bytesPerPixel;
        for (int row = 0; row < height; row++) {
            const auto* src = pixels + static_cast<size_t>(row) * srcRowPitch;
            auto* dst = static_cast<uint8_t*>(mapped) + static_cast<size_t>(row) * copyFootprint.rowPitch;
            memcpy(dst, src, srcRowPitch);
        }
        stagingBuffer->Unmap();

        const auto commandBuffer = device.CreateCommandBuffer(RHI::QueueType::graphics);
        const auto commandRecorder = commandBuffer->Begin();
        {
            const auto copyRecorder = commandRecorder->BeginCopyPass();
            copyRecorder->ResourceBarrier(RHI::Barrier::Transition(imguiFontTexture.Get(), RHI::TextureState::undefined, RHI::TextureState::copyDst));
            copyRecorder->CopyBufferToTexture(
                stagingBuffer.Get(),
                imguiFontTexture.Get(),
                RHI::BufferTextureCopyInfo(
                    0,
                    RHI::TextureSubResourceInfo(),
                    Common::UVec3Consts::zero,
                    Common::UVec3(static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1),
                    copyFootprint.rowPitch,
                    copyFootprint.slicePitch));
            copyRecorder->ResourceBarrier(RHI::Barrier::Transition(imguiFontTexture.Get(), RHI::TextureState::copyDst, RHI::TextureState::shaderReadOnly));
            copyRecorder->EndPass();
        }
        commandRecorder->End();

        const auto fence = device.CreateFence(false);
        device.GetQueue(RHI::QueueType::graphics, 0)->Submit(commandBuffer.Get(), RHI::QueueSubmitInfo().SetSignalFence(fence.Get()));
        fence->Wait();
        ImGui::GetIO().Fonts->SetTexID(static_cast<ImTextureID>(reinterpret_cast<uintptr_t>(imguiFontTextureView.Get())));
    }

    void ImGuiRhiRenderer::CreateImGuiDynamicBuffers(int inVertexCount, int inIndexCount)
    {
        imguiVertexBufferCapacity = std::max(1, inVertexCount);
        imguiIndexBufferCapacity = std::max(1, inIndexCount);

        const uint32_t vertexBufferSize = static_cast<uint32_t>(imguiVertexBufferCapacity * sizeof(ImDrawVert));
        imguiVertexBuffer = device.CreateBuffer(
            RHI::BufferCreateInfo()
                .SetSize(vertexBufferSize)
                .SetUsages(RHI::BufferUsageBits::vertex | RHI::BufferUsageBits::mapWrite | RHI::BufferUsageBits::copySrc)
                .SetInitialState(RHI::BufferState::staging)
                .SetDebugName("imguiVertexBuffer"));
        imguiVertexBufferView = imguiVertexBuffer->CreateBufferView(
            RHI::BufferViewCreateInfo()
                .SetType(RHI::BufferViewType::vertex)
                .SetSizeInBytes(vertexBufferSize)
                .SetOffsetInBytes(0)
                .SetExtendVertex(sizeof(ImDrawVert)));

        const uint32_t indexBufferSize = static_cast<uint32_t>(imguiIndexBufferCapacity * sizeof(ImDrawIdx));
        imguiIndexBuffer = device.CreateBuffer(
            RHI::BufferCreateInfo()
                .SetSize(indexBufferSize)
                .SetUsages(RHI::BufferUsageBits::index | RHI::BufferUsageBits::mapWrite | RHI::BufferUsageBits::copySrc)
                .SetInitialState(RHI::BufferState::staging)
                .SetDebugName("imguiIndexBuffer"));
        imguiIndexBufferView = imguiIndexBuffer->CreateBufferView(
            RHI::BufferViewCreateInfo()
                .SetType(RHI::BufferViewType::index)
                .SetSizeInBytes(indexBufferSize)
                .SetOffsetInBytes(0)
                .SetExtendIndex(ImGuiRendering::Internal::imguiIndexFormat));
    }

    void ImGuiRhiRenderer::UpdateImGuiBuffers(ImDrawData& inDrawData)
    {
        auto* vertexDst = static_cast<ImDrawVert*>(imguiVertexBuffer->Map(RHI::MapMode::write, 0, inDrawData.TotalVtxCount * sizeof(ImDrawVert)));
        auto* indexDst = static_cast<ImDrawIdx*>(imguiIndexBuffer->Map(RHI::MapMode::write, 0, inDrawData.TotalIdxCount * sizeof(ImDrawIdx)));
        for (int i = 0; i < inDrawData.CmdListsCount; i++) {
            const ImDrawList* commandList = inDrawData.CmdLists[i];
            memcpy(vertexDst, commandList->VtxBuffer.Data, static_cast<size_t>(commandList->VtxBuffer.Size) * sizeof(ImDrawVert));
            memcpy(indexDst, commandList->IdxBuffer.Data, static_cast<size_t>(commandList->IdxBuffer.Size) * sizeof(ImDrawIdx));
            vertexDst += commandList->VtxBuffer.Size;
            indexDst += commandList->IdxBuffer.Size;
        }
        imguiVertexBuffer->Unmap();
        imguiIndexBuffer->Unmap();
    }

    void ImGuiRhiRenderer::UpdateImGuiPassParams(const ImDrawData& inDrawData)
    {
        const Common::Internal::ImGuiPassParams params {
            Common::FVec2(inDrawData.DisplayPos.x, inDrawData.DisplayPos.y),
            Common::FVec2(inDrawData.DisplaySize.x, inDrawData.DisplaySize.y)
        };
        auto* data = imguiPassParamsBuffer->Map(RHI::MapMode::write, 0, sizeof(Common::Internal::ImGuiPassParams));
        memcpy(data, &params, sizeof(Common::Internal::ImGuiPassParams));
        imguiPassParamsBuffer->Unmap();
    }

    RHI::BindGroup* ImGuiRhiRenderer::GetOrCreateImGuiTextureBindGroup(RHI::TextureView& inTextureView)
    {
        if (const auto iter = imguiFrameTextureBindGroups.find(&inTextureView);
            iter != imguiFrameTextureBindGroups.end()) {
            return iter->second.Get();
        }

        auto bindGroup = device.CreateBindGroup(
            RHI::BindGroupCreateInfo(imguiBindGroupLayout.Get(), "imguiBindGroup")
                .AddEntry(RHI::BindGroupEntry(imguiPixelShaderCompileOutput.reflectionData.QueryResourceBindingChecked("imageTex").second, RHI::ShaderStageBits::sPixel, &inTextureView))
                .AddEntry(RHI::BindGroupEntry(imguiPixelShaderCompileOutput.reflectionData.QueryResourceBindingChecked("imageSampler").second, RHI::ShaderStageBits::sPixel, imguiImageSampler.Get()))
                .AddEntry(RHI::BindGroupEntry(imguiVertexShaderCompileOutput.reflectionData.QueryResourceBindingChecked("passParams").second, RHI::ShaderStageBits::sVertex, imguiPassParamsBufferView.Get())));
        auto* result = bindGroup.Get();
        imguiFrameTextureBindGroups.emplace(&inTextureView, std::move(bindGroup));
        return result;
    }

}
