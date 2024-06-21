//
// Created by swtpotato on 2024/6/21.
//
#include <iostream>
#include <vector>
#include <Application.h>

using namespace RHI;

class ParallelCompute final : public Application {
public:
    NonCopyable(ParallelCompute)
    explicit ParallelCompute(const std::string& name)
        :Application(name)
    {}
    ~ParallelCompute() override = default;

    void Initialize(int argc, char* argv[])
    {
       //  std::string rhiString;
       //  if (const auto cli = (
       //          clipp::option("-w").doc("window width, 1024 by default") & clipp::value("width", windowExtent.x),
       //          clipp::option("-h").doc("window height, 768 by default") & clipp::value("height", windowExtent.y),
       //          clipp::required("-rhi").doc("RHI type, can be 'dx12' or 'vulkan'") & clipp::value("RHI type", rhiString));
       //      !clipp::parse(argc, argv, cli)) {
       //      std::cout << clipp::make_man_page(cli, argv[0]);
       //      return -1;
       // }
       //
       //  rhiType = RHI::RHIAbbrStringToRHIType(rhiString);
       //  instance = RHI::Instance::GetByType(rhiType);

        SelectGPU();
        RequestDeviceAndFetchQueues();
        PrepareDataAndCreateGPURes();
        DealWithPipelineBindingOBjs();
        CreateCmdBuffAndSyncObj();
    }

    void ComputeAndShow()
    {
        BuildCmdBufferAndSubmit();

    }
private:
    void SelectGPU()
    {
        gpu = GetRHIInstance()->GetGpu(0);
    }

    void RequestDeviceAndFetchQueues()
    {
        device = gpu->RequestDevice(DeviceCreateInfo().AddQueueRequest(QueueRequestInfo(QueueType::compute, 1)));
        computeQueue = device->GetQueue(QueueType::compute, 0);
    }

    void PrepareDataAndCreateGPURes()
    {
        std::vector<Data> data(dataDim * dataDim);
        for(int i = 0; i < dataDim * dataDim; ++i)
        {
            data[i].v1 = FVec3(i + 1, i - 1, i);
            data[i].v2 = FVec2(i, i + 1);
        }

        const auto bufInfo = BufferCreateInfo()
            .SetSize(data.size() * sizeof(Data))
            .SetUsages(BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .SetInitialState(BufferState::staging);

        const UniqueRef<Buffer> immediateBuf = device->CreateBuffer(bufInfo);
        if (immediateBuf != nullptr) {
            auto* mapPointer = immediateBuf->Map(MapMode::write, 0, bufInfo.size);
            memcpy(mapPointer, data.data(), bufInfo.size);
            immediateBuf->UnMap();
        }

        const auto texInfo = TextureCreateInfo()
            .SetFormat(PixelFormat::rgba32Float)
            .SetMipLevels(1)
            .SetWidth(dataDim)
            .SetHeight(dataDim)
            .SetDepthOrArraySize(1)
            .SetDimension(TextureDimension::t2D)
            .SetSamples(1)
            .SetUsages(TextureUsageBits::copyDst | TextureUsageBits::textureBinding)
            .SetInitialState(TextureState::undefined);
        inputBuffer = device->CreateTexture(texInfo);

        const auto texViewInfo = TextureViewCreateInfo()
            .SetDimension(TextureViewDimension::tv2D)
            .SetMipLevels(0, 1)
            .SetArrayLayers(0, 1)
            .SetAspect(TextureAspect::color)
            .SetType(TextureViewType::textureBinding);
        inputBufferView = inputBuffer->CreateTextureView(texViewInfo);

        const UniqueRef<CommandBuffer> copyCmd = device->CreateCommandBuffer();
        const UniqueRef<CommandRecorder> recorder = copyCmd->Begin();
        {
            const UniqueRef<CopyPassCommandRecorder> copyRecorder = recorder->BeginCopyPass();
            copyRecorder->ResourceBarrier(Barrier::Transition(inputBuffer.Get(), TextureState::undefined, TextureState::copyDst));
            copyRecorder->CopyBufferToTexture(
                immediateBuf.Get(),
                inputBuffer.Get(),
                BufferTextureCopyInfo(0, TextureSubResourceInfo(), UVec3Consts::zero, UVec3(dataDim, dataDim, 1)));
            copyRecorder->ResourceBarrier(Barrier::Transition(inputBuffer.Get(), TextureState::copyDst, TextureState::shaderReadOnly));
            copyRecorder->EndPass();
        }
        recorder->End();

        const UniqueRef<Fence> immediateFence = device->CreateFence(false);
        QueueSubmitInfo submitInfo {};
        submitInfo.signalFence = immediateFence.Get();
        computeQueue->Submit(copyCmd.Get(), submitInfo);
        immediateFence->Wait();

        const auto outputBufferInfo = BufferCreateInfo()
            .SetSize(data.size() * sizeof(Data))
            .SetUsages(BufferUsageBits::storage | BufferUsageBits::copySrc)
            .SetInitialState(BufferState::undefined);
        outputBuffer = device->CreateBuffer(outputBufferInfo);

        const auto outputBufferViewInfo = BufferViewCreateInfo()
            .SetType(BufferViewType::storageBinding)
            .SetSize(data.size() * sizeof(Data))
            .SetOffset(0);
        outputBufferView = outputBuffer->CreateBufferView(outputBufferViewInfo);

        const auto readbackBufferInfo = BufferCreateInfo()
            .SetSize(data.size() * sizeof(Data))
            .SetUsages(BufferUsageBits::mapRead | BufferUsageBits::copyDst)
            .SetInitialState(BufferState::undefined);
        readbackBuffer = device->CreateBuffer(readbackBufferInfo);
    }

    void DealWithPipelineBindingOBjs()
    {
        csOutPut = CompileShader("../Test/Sample/RHI/ParallelCompute/Compute.hlsl", "CSMain", ShaderStageBits::sCompute);
        csShaderModule = device->CreateShaderModule(ShaderModuleCreateInfo('CSMain', csOutPut.byteCode));

        const auto layoutCreateInfo = BindGroupLayoutCreateInfo(0)
            .AddEntry(BindGroupLayoutEntry(csOutPut.reflectionData.QueryResourceBindingChecked("input").second, ShaderStageBits::sCompute))
            .AddEntry(BindGroupLayoutEntry(csOutPut.reflectionData.QueryResourceBindingChecked("output").second, ShaderStageBits::sCompute));
        bindGroupLayout = device->CreateBindGroupLayout(layoutCreateInfo);

        const auto bindGroupCreateInfo = BindGroupCreateInfo(bindGroupLayout.Get())
            .AddEntry(BindGroupEntry(csOutPut.reflectionData.QueryResourceBindingChecked("input").second, inputBufferView.Get()))
            .AddEntry(BindGroupEntry(csOutPut.reflectionData.QueryResourceBindingChecked("output").second, outputBufferView.Get()));
        bindGroup = device->CreateBindGroup(bindGroupCreateInfo);

        pipelineLayout = device->CreatePipelineLayout(PipelineLayoutCreateInfo().AddBindGroupLayout(bindGroupLayout.Get()));
    }

    void createPipeline()
    {
        const auto pipelineInfo = ComputePipelineCreateInfo()
            .SetLayout(pipelineLayout.Get())
            .SetComputeShader(csShaderModule.Get());

        pipeline = device->CreateComputePipeline(pipelineInfo);
    }

    void CreateCmdBuffAndSyncObj()
    {
        commandBuffer = device->CreateCommandBuffer();
        fence = device->CreateFence(false);
    }

    void BuildCmdBufferAndSubmit()
    {
        UniqueRef<CommandRecorder> recorder = commandBuffer->Begin();

        // do compute work
        UniqueRef<ComputePassCommandRecorder> computeRecorder = recorder->BeginComputePass();
        computeRecorder->SetPipeline(pipeline.Get());
        computeRecorder->SetBindGroup(0, bindGroup.Get());
        computeRecorder->Dispatch(1, 1, 1);
        computeRecorder->EndPass();

        // read back to host buffer
        UniqueRef<CopyPassCommandRecorder> copyRecorder = recorder->BeginCopyPass();
        copyRecorder->ResourceBarrier(Barrier::Transition(outputBuffer.Get(), BufferState::undefined, BufferState::copySrc));
        copyRecorder->CopyBufferToBuffer(
            outputBuffer.Get(),
            readbackBuffer.Get(),
            BufferCopyInfo(0, 0, dataDim * dataDim * sizeof(Data)));
        copyRecorder->ResourceBarrier(Barrier::Transition(inputBuffer.Get(), TextureState::copyDst, TextureState::shaderReadOnly));
        copyRecorder->EndPass();

        recorder->End();

        QueueSubmitInfo submitInfo {};
        submitInfo.signalFence = fence.Get();
        computeQueue->Submit(commandBuffer.Get(), submitInfo);
        fence->Wait();
    }

    struct Data {
        FVec3 v1;
        FVec2 v2;
    };
    const int dataDim = 64;

    Gpu* gpu = nullptr;
    UniqueRef<Device> device;
    Queue* computeQueue = nullptr;
    UniqueRef<Texture> inputBuffer;
    UniqueRef<TextureView> inputBufferView;
    UniqueRef<Buffer> outputBuffer;
    UniqueRef<BufferView> outputBufferView;
    UniqueRef<Buffer> readbackBuffer;
    UniqueRef<BufferView> readbackBufferView;
    UniqueRef<BindGroupLayout> bindGroupLayout;
    UniqueRef<BindGroup> bindGroup;
    UniqueRef<Sampler> sampler;
    UniqueRef<Buffer> uniformBuffer;
    UniqueRef<BufferView> uniformBufferView;
    UniqueRef<CommandBuffer> cmdBuffer;
    UniqueRef<PipelineLayout> pipelineLayout;
    UniqueRef<ComputePipeline> pipeline;
    UniqueRef<ShaderModule> computeShader;
    UniqueRef<Fence> fence;
    ShaderCompileOutput csOutPut;
    UniqueRef<ShaderModule> csShaderModule;
    UniqueRef<CommandBuffer> commandBuffer;
};

int main(int argc, char* argv[])
{
    ParallelCompute app("RHI-ParallelCompute");
    app.Initialize();
    app.ComputeAndShow();

    return 0;
}
