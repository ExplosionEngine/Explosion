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

    void ComputeAndSaveResult()
    {
        BuildCmdBufferAndSubmit();

        // Map the data so we can read it on CPU.
        const auto* mappedData = static_cast<PackedVec*>(readbackBuffer->Map(MapMode::read, 0, dataNum * sizeof(PackedVec)));

        std::ofstream fout("results.txt");
        Assert(fout.is_open());

        for(int i = 0; i < dataNum; ++i)
        {
            std::cout << "("
            << mappedData[i].v1.x << ", " << mappedData[i].v1.y << ", " << mappedData[i].v1.z << ", " << mappedData[i].v1.w << ", "
            << mappedData[i].v2.x << ", " << mappedData[i].v2.y << ", " << mappedData[i].v2.z << ", " << mappedData[i].v2.w
            << ")" << '\n';
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
        device = gpu->RequestDevice(DeviceCreateInfo().AddQueueRequest(QueueRequestInfo(QueueType::graphics, 1)));
        queue = device->GetQueue(QueueType::graphics, 0);
    }

    void PrepareDataAndCreateGPURes()
    {
        std::vector<PackedVec> data(dataNum);
        for(int i = 0; i < dataNum; ++i)
        {
            data[i].v1 = FVec4(i - 1);
            data[i].v2 = FVec4(i + 1);
        }

        const auto bufInfo = BufferCreateInfo()
            .SetSize(data.size() * sizeof(PackedVec))
            .SetUsages(BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .SetInitialState(BufferState::staging);

        const UniquePtr<Buffer> stagingBuf = device->CreateBuffer(bufInfo);
        if (stagingBuf != nullptr) {
            auto* mapPointer = stagingBuf->Map(MapMode::write, 0, bufInfo.size);
            memcpy(mapPointer, data.data(), bufInfo.size);
            stagingBuf->UnMap();
        }

        const auto inputBufInfo = BufferCreateInfo()
            .SetSize(data.size() * sizeof(PackedVec))
            .SetUsages(BufferUsageBits::copyDst | BufferUsageBits::storage)
            .SetInitialState(BufferState::undefined);
        inputBuffer = device->CreateBuffer(inputBufInfo);

        const auto inputBufViewInfo = BufferViewCreateInfo()
            .SetType(BufferViewType::storageBinding)
            .SetSize(data.size() * sizeof(PackedVec))
            .SetOffset(0)
            .SetExtendStorage(sizeof(PackedVec));
        inputBufferView = inputBuffer->CreateBufferView(inputBufViewInfo);

        const UniquePtr<CommandBuffer> copyCmd = device->CreateCommandBuffer();
        const UniquePtr<CommandRecorder> recorder = copyCmd->Begin();
        {
            const UniquePtr<CopyPassCommandRecorder> copyRecorder = recorder->BeginCopyPass();
            copyRecorder->ResourceBarrier(Barrier::Transition(inputBuffer.Get(), BufferState::undefined, BufferState::copyDst));
            copyRecorder->CopyBufferToBuffer(
                stagingBuf.Get(),
                inputBuffer.Get(),
                BufferCopyInfo(0, 0, data.size() * sizeof(PackedVec)));
            copyRecorder->ResourceBarrier(Barrier::Transition(inputBuffer.Get(), BufferState::copyDst, BufferState::shaderReadOnly));
            copyRecorder->EndPass();
        }
        recorder->End();

        const UniquePtr<Fence> mFence = device->CreateFence(false);
        QueueSubmitInfo submitInfo {};
        submitInfo.signalFence = mFence.Get();
        queue->Submit(copyCmd.Get(), submitInfo);
        mFence->Wait();

        const auto outputBufferInfo = BufferCreateInfo()
            .SetSize(data.size() * sizeof(PackedVec))
            .SetUsages(BufferUsageBits::rwStorage | BufferUsageBits::copySrc)
            .SetInitialState(BufferState::rwStorage);
        outputBuffer = device->CreateBuffer(outputBufferInfo);

        const auto outputBufferViewInfo = BufferViewCreateInfo()
            .SetType(BufferViewType::rwStorageBinding)
            .SetSize(data.size() * sizeof(PackedVec))
            .SetOffset(0)
            .SetExtendStorage(sizeof(PackedVec));
        outputBufferView = outputBuffer->CreateBufferView(outputBufferViewInfo);

        const auto readbackBufferInfo = BufferCreateInfo()
            .SetSize(data.size() * sizeof(PackedVec))
            .SetUsages(BufferUsageBits::mapRead | BufferUsageBits::copyDst)
            .SetInitialState(BufferState::copyDst);
        readbackBuffer = device->CreateBuffer(readbackBufferInfo);
    }

    void DealWithPipelineBindingOBjs()
    {
        csOutPut = CompileShader("../Test/Sample/RHI-ParallelCompute/Compute.hlsl", "CSMain", ShaderStageBits::sCompute);

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
        UniquePtr<CommandRecorder> recorder = commandBuffer->Begin();

        // do compute work
        UniquePtr<ComputePassCommandRecorder> computeRecorder = recorder->BeginComputePass();
        computeRecorder->SetPipeline(pipeline.Get());
        computeRecorder->SetBindGroup(0, bindGroup.Get());
        computeRecorder->Dispatch(1, 1, 1);
        computeRecorder->EndPass();

        // read back to host buffer
        UniquePtr<CopyPassCommandRecorder> copyRecorder = recorder->BeginCopyPass();
        copyRecorder->ResourceBarrier(Barrier::Transition(outputBuffer.Get(), BufferState::rwStorage, BufferState::copySrc));
        copyRecorder->CopyBufferToBuffer(
            outputBuffer.Get(),
            readbackBuffer.Get(),
            BufferCopyInfo(0, 0, dataNum * sizeof(PackedVec)));
        copyRecorder->EndPass();

        recorder->End();

        QueueSubmitInfo submitInfo {};
        submitInfo.signalFence = fence.Get();
        queue->Submit(commandBuffer.Get(), submitInfo);
        fence->Wait();
    }

    struct PackedVec {
        FVec4 v1;
        FVec4 v2;
    };

    const int dataNum = 32;

    Gpu* gpu = nullptr;
    UniquePtr<Device> device;
    Queue* queue = nullptr;
    UniquePtr<Buffer> inputBuffer;
    UniquePtr<BufferView> inputBufferView;
    UniquePtr<Buffer> outputBuffer;
    UniquePtr<BufferView> outputBufferView;
    UniquePtr<Buffer> readbackBuffer;
    UniquePtr<BindGroupLayout> bindGroupLayout;
    UniquePtr<BindGroup> bindGroup;
    UniquePtr<Sampler> sampler;
    UniquePtr<CommandBuffer> cmdBuffer;
    UniquePtr<PipelineLayout> pipelineLayout;
    UniquePtr<ComputePipeline> pipeline;
    UniquePtr<ShaderModule> computeShader;
    UniquePtr<Fence> fence;
    ShaderCompileOutput csOutPut;
    UniquePtr<ShaderModule> csShaderModule;
    UniquePtr<CommandBuffer> commandBuffer;
};

int main(int argc, char* argv[])
{
    ParallelCompute application("RHI-ParallelCompute");
    if (!application.Initialize(argc, argv)) {
        return -1;
    }
    application.Setup();
    application.ComputeAndSaveResult();

    return 0;
}