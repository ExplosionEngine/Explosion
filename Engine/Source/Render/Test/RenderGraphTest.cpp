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
        ShaderStage::sCompute);

    VariantSet();
    DefaultVariantFilter
};
RegisterGlobalShader(RenderGraphTestCS);

class TestComputePass : public RGComputePass {
public:
    TestComputePass() : RGComputePass("TestComputePass") {}

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

        RGBufferDesc bufferDesc;
        bufferDesc.size = sizeof(Parameters);
        bufferDesc.usages = RHI::BufferUsageBits::uniform;
        bufferDesc.debugName = "TestUniformBuffer";
        uniformBuffer = builder.CreateBuffer("TestUniformBuffer", bufferDesc);

        RGTextureDesc textureDesc;
        textureDesc.dimension = RHI::TextureDimension::t2D;
        textureDesc.extent = { 1024, 1024 };
        textureDesc.format = RHI::PixelFormat::bgra8Unorm;
        textureDesc.usages = RHI::TextureUsageBits::storageBinding;
        textureDesc.mipLevels = 1;
        textureDesc.samples = 1;
        textureDesc.initialState = RHI::TextureState::undefined;
        outputTexture = builder.CreateTexture("TestOutputTexture", textureDesc);

        RGBufferViewDesc bufferViewDesc;
        bufferViewDesc.type = RHI::BufferViewType::uniformBinding;
        bufferViewDesc.size = sizeof(Parameters);
        bufferViewDesc.offset = 0;
        auto* uniformBufferView = builder.CreateBufferView(uniformBuffer, bufferViewDesc);

        RGTextureViewDesc textureViewDesc;
        textureViewDesc.type = RHI::TextureViewType::storageBinding;
        textureViewDesc.dimension = RHI::TextureViewDimension::tv2D;
        textureViewDesc.aspect = RHI::TextureAspect::color;
        textureViewDesc.baseArrayLayer = 0;
        textureViewDesc.arrayLayerNum = 1;
        textureViewDesc.baseMipLevel = 0;
        textureViewDesc.mipLevelNum = 1;
        auto* outputTextureView = builder.CreateTextureView(outputTexture, textureViewDesc);

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
        instance = RHI::Instance::GetByType(RHI::RHIType::dummy);

        RHI::QueueInfo queueInfo {};
        queueInfo.type = RHI::QueueType::graphics;
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
    renderGraph.Execute(mainFence.Get());
    mainFence->Wait();
}
