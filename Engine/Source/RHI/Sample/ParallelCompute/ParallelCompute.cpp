//
// Created by swtpotato on 2024/6/21.
//

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

    void Initialize()
    {

    }

    void ComputeAndShow()
    {

    }
private:
    void SelectGPU()
    {
        gpu = instance->GetGpu(0);
    }

    void RequestDeviceAndFetchQueues()
    {
        device = gpu->RequestDevice(DeviceCreateInfo().AddQueueRequest(QueueRequestInfo(QueueType::compute, 1)));
        computeQueue = device->GetQueue(QueueType::compute, 0);
    }

    void PrepareDataAndCreateBuffer()
    {
        std::vector<Data> dataA(dataNum);
        std::vector<Data> dataB(dataNum);
        for(int i = 0; i < dataNum; ++i)
        {
            dataA[i].v1 = FVec3(i, i, i);
            dataA[i].v2 = FVec2(i, 0);

            dataB[i].v1 = FVec3(-i, i, 0.0f);
            dataB[i].v2 = FVec2(0, -i);
        }
    }

    void CreateBindGroupLayout()
    {

    }

    void CreateBindGroup()
    {

    }

    void createPipeline()
    {

    }

    void CreateCmdbuffAndSyncObj()
    {

    }

    void BuildCmdBuffer()
    {

    }

    struct Data {
        FVec3 v1;
        FVec2 v2;
    };
    const int dataNum = 32;

    Gpu* gpu = nullptr;
    UniqueRef<Device> device;
    Queue* computeQueue = nullptr;
    UniqueRef<Texture> texA;
    UniqueRef<TextureView> texViewA;
    UniqueRef<Texture> texB;
    UniqueRef<TextureView> texViewB;
    UniqueRef<Buffer> outBuffer;
    UniqueRef<BufferView> outBufferView;
    UniqueRef<BindGroupLayout> bindGroupLayout;
    UniqueRef<BindGroup> bindGroup;
    UniqueRef<Sampler> sampler;
    UniqueRef<Buffer> uniformBuffer;
    UniqueRef<BufferView> uniformBufferView;
    UniqueRef<CommandBuffer> cmdBuffer;
    UniqueRef<PipelineLayout> pipelineLayout;
    UniqueRef<RasterPipeline> pipeline;
    UniqueRef<ShaderModule> computeShader;
    UniqueRef<Fence> fence;
};

int main(int argc, char* argv[])
{
    ParallelCompute app("RHI-ParallelCompute");
    return app.Run(argc, argv);
}
