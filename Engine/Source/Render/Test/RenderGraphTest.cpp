//
// Created by johnk on 2023/3/22.
//

#include <gtest/gtest.h>

#include <RHI/RHI.h>
#include <Render/RenderGraph.h>
#include <Render/Pipeline.h>
#include <Render/Shader.h>

using namespace Render;

class RenderGraphTestCS : public GlobalShader {
public:
    ShaderInfo(
        "RenderGraphTestCS",
        "/Engine/Shader/Test/RenderGraphTest.esl",
        "CSMain",
        ShaderStage::S_COMPUTE);

    VariantSet();
    DefaultVariantFilter
};
RegisterGlobalShader(RenderGraphTestCS);

class TestComputePass : public RGComputePass {
protected:
    void Setup(RGComputePassBuilder& builder) override
    {
        RHI::Device& device = builder.GetDevice();

        RenderGraphTestCS::VariantSet variantSet;
        ShaderInstance testCS = GlobalShaderMap<RenderGraphTestCS>::Get(device).GetShaderInstance(variantSet);
        ComputePipelineShaderSet shaders = { testCS };

        ComputePipelineStateDesc pipelineDesc;
        pipelineDesc.shaders = shaders;
        pipeline = PipelineCache::Get(device).GetPipeline(pipelineDesc);

        builder.SetAsyncCompute(false);

        uniformBuffer = builder.CreateBuffer("TestUniformBuffer", RGBufferDesc::Create(sizeof(Parameters), RHI::BufferUsageBits::UNIFORM));
        outputTexture = builder.CreateTexture("TestOutputTexture", RGTextureDesc::Create2D(1024, 1024, RHI::PixelFormat::BGRA8_UNORM, RHI::TextureUsageBits::STORAGE_BINDING));
        auto* uniformBufferView = builder.CreateBufferView(RGBufferViewDesc::Create(uniformBuffer));
        auto* outputTextureView = builder.CreateTextureView(RGTextureViewDesc::Create2D(outputTexture));

        builder.MarkAsConsumed(outputTexture);
        bindGroup = builder.AllocateBindGroup(RGBindGroupDesc::Create(
            pipeline->GetBindGroupLayout(0),
            RGBindItem::UniformBuffer("inputBuffer", uniformBufferView),
            RGBindItem::StorageTexture("outputTexture", outputTextureView)));
    }

    void Execute(RHI::ComputePassCommandEncoder& encoder) override
    {
        parameters.frameCount++;
        uniformBuffer->UploadData(parameters);

        encoder.SetPipeline(pipeline->GetRHI());
        encoder.SetBindGroup(0, bindGroup->GetRHI());
        encoder.Dispatch(8, 8, 1);
    }

private:
    struct Parameters {
        size_t frameCount = 0;
    } parameters;

    RGBuffer* uniformBuffer;
    RGTexture* outputTexture;
    RGBindGroup* bindGroup;
    ComputePipelineState* pipeline;
};

struct RenderGraphTest : public testing::Test {
    void SetUp() override
    {
        instance = RHI::Instance::GetByType(RHI::RHIType::DUMMY);

        RHI::QueueInfo queueInfo {};
        queueInfo.type = RHI::QueueType::GRAPHICS;
        queueInfo.num = 1;
        RHI::DeviceCreateInfo deviceCreateInfo {};
        deviceCreateInfo.queueCreateInfoNum = 1;
        deviceCreateInfo.queueCreateInfos = &queueInfo;
        device = instance->GetGpu(0)->RequestDevice(deviceCreateInfo);
    }

    void TearDown() override {}

    RHI::Instance* instance;
    Common::UniqueRef<RHI::Device> device;
};

TEST_F(RenderGraphTest, BasicTest)
{
    Common::UniqueRef<RHI::Fence> mainFence = device->CreateFence();

    RenderGraph renderGraph(*device);
    renderGraph.Setup();
    renderGraph.Compile();
    renderGraph.Execute(mainFence.Get(), nullptr);
    mainFence->Wait();
}

TEST_F(RenderGraphTest, ComputePassTest)
{
    Common::UniqueRef<RHI::Fence> mainFence = device->CreateFence();

    RenderGraph renderGraph(*device);
    renderGraph.Setup();
    renderGraph.Compile();
    renderGraph.Execute(mainFence.Get(), nullptr);
    mainFence->Wait();
}
