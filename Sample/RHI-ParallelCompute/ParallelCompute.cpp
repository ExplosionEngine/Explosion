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

    void Setup()
    {
        SelectGPU();
        RequestDeviceAndFetchQueues();
        PrepareDataAndCreateGPURes();
        DealWithPipelineBindingOBjs();
        CreatePipeline();
        CreateCmdBuffAndSyncObj();
    }

    void ComputeAndShow()
    {
        BuildCmdBufferAndSubmit();

        // Map the data so we can read it on CPU.
        const auto* mappedData = static_cast<FVec4*>(readbackBuffer->Map(MapMode::read, 0, dataNum * sizeof(FVec4)));

        std::ofstream fout("results.txt");

        for(int i = 0; i < dataNum; ++i)
        {
            std::cout << "(" << mappedData[i].x << ", " << mappedData[i].y << ", " <<
                 ", " << mappedData[i].z << ", " << mappedData[i].w << ")" << '\n';
        }

        readbackBuffer->UnMap();
    }
private:
    void SelectGPU()
    {
        gpu = GetRHIInstance()->GetGpu(0);
    }

    void RequestDeviceAndFetchQueues()
    {
        // Generally, there is a queue in GPU doing both graphics and compute work
        // TODO: Is it necessary to make a distinction between the composite-use-queue and the single-use-queue?
        device = gpu->RequestDevice(DeviceCreateInfo().AddQueueRequest(QueueRequestInfo(QueueType::graphics, 1)));
        queue = device->GetQueue(QueueType::graphics, 0);
    }

    void PrepareDataAndCreateGPURes()
    {
        std::vector<FVec4> data(dataNum);
        for(int i = 0; i < dataNum; ++i)
        {
            data[i] = FVec4(i - 2, i - 1, i, i + 1);
        }

        const auto bufInfo = BufferCreateInfo()
            .SetSize(data.size() * sizeof(FVec4))
            .SetUsages(BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .SetInitialState(BufferState::staging);

        const UniqueRef<Buffer> stagingBuf = device->CreateBuffer(bufInfo);
        if (stagingBuf != nullptr) {
            auto* mapPointer = stagingBuf->Map(MapMode::write, 0, bufInfo.size);
            memcpy(mapPointer, data.data(), bufInfo.size);
            stagingBuf->UnMap();
        }

        const auto inputBufInfo = BufferCreateInfo()
            .SetSize(data.size() * sizeof(FVec4))
            .SetUsages(BufferUsageBits::copyDst | BufferUsageBits::uniform)
            .SetInitialState(BufferState::undefined);
        inputBuffer = device->CreateBuffer(inputBufInfo);

        const auto inputBufViewInfo = BufferViewCreateInfo()
            .SetType(BufferViewType::uniformBinding)
            .SetSize(data.size() * sizeof(FVec4))
            .SetOffset(0);
        inputBufferView = inputBuffer->CreateBufferView(inputBufViewInfo);

        const UniqueRef<CommandBuffer> copyCmd = device->CreateCommandBuffer();
        const UniqueRef<CommandRecorder> recorder = copyCmd->Begin();
        {
            const UniqueRef<CopyPassCommandRecorder> copyRecorder = recorder->BeginCopyPass();
            copyRecorder->ResourceBarrier(Barrier::Transition(inputBuffer.Get(), BufferState::undefined, BufferState::copyDst));
            copyRecorder->CopyBufferToBuffer(
                stagingBuf.Get(),
                inputBuffer.Get(),
                BufferCopyInfo(0, 0, data.size() * sizeof(FVec4)));
            copyRecorder->ResourceBarrier(Barrier::Transition(inputBuffer.Get(), BufferState::copyDst, BufferState::shaderReadOnly));
            copyRecorder->EndPass();
        }
        recorder->End();

        const UniqueRef<Fence> mFence = device->CreateFence(false);
        QueueSubmitInfo submitInfo {};
        submitInfo.signalFence = mFence.Get();
        queue->Submit(copyCmd.Get(), submitInfo);
        mFence->Wait();

        const auto outputBufferInfo = BufferCreateInfo()
            .SetSize(data.size() * sizeof(FVec4))
            .SetUsages(BufferUsageBits::storage | BufferUsageBits::copySrc)
            .SetInitialState(BufferState::storage);
        outputBuffer = device->CreateBuffer(outputBufferInfo);

        const auto outputBufferViewInfo = BufferViewCreateInfo()
            .SetType(BufferViewType::storageBinding)
            .SetSize(data.size() * sizeof(FVec4))
            .SetOffset(0)
            .SetExtendStorage(sizeof(float), StorageFormat::float32);
        outputBufferView = outputBuffer->CreateBufferView(outputBufferViewInfo);

        const auto readbackBufferInfo = BufferCreateInfo()
            .SetSize(data.size() * sizeof(FVec4))
            .SetUsages(BufferUsageBits::mapRead | BufferUsageBits::copyDst)
            .SetInitialState(BufferState::copyDst);
        readbackBuffer = device->CreateBuffer(readbackBufferInfo);
    }

    void DealWithPipelineBindingOBjs()
    {
        csOutPut = CompileShader("../Test/Sample/RHI/ParallelCompute/Compute.hlsl", "CSMain", ShaderStageBits::sCompute);

        auto shaderModuleCreateInfo = ShaderModuleCreateInfo("CSMain", csOutPut.byteCode);
        csShaderModule = device->CreateShaderModule(shaderModuleCreateInfo);

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

    void CreatePipeline()
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
        copyRecorder->ResourceBarrier(Barrier::Transition(outputBuffer.Get(), BufferState::storage, BufferState::copySrc));
        copyRecorder->CopyBufferToBuffer(
            outputBuffer.Get(),
            readbackBuffer.Get(),
            BufferCopyInfo(0, 0, dataNum * sizeof(FVec4)));
        copyRecorder->EndPass();

        recorder->End();

        QueueSubmitInfo submitInfo {};
        submitInfo.signalFence = fence.Get();
        queue->Submit(commandBuffer.Get(), submitInfo);
        fence->Wait();
    }

    const int dataNum = 16;

    Gpu* gpu = nullptr;
    UniqueRef<Device> device;
    Queue* queue = nullptr;
    UniqueRef<Buffer> inputBuffer;
    UniqueRef<BufferView> inputBufferView;
    UniqueRef<Buffer> outputBuffer;
    UniqueRef<BufferView> outputBufferView;
    UniqueRef<Buffer> readbackBuffer;
    UniqueRef<BindGroupLayout> bindGroupLayout;
    UniqueRef<BindGroup> bindGroup;
    UniqueRef<Sampler> sampler;
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
    ParallelCompute application("RHI-ParallelCompute");
    if (!application.Initialize(argc, argv)) {
        return -1;
    }
    application.Setup();
    application.ComputeAndShow();

    return 0;
}