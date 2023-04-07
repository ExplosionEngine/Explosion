//
// Created by johnk on 2023/3/22.
//

#include <gtest/gtest.h>

#include <RHI/RHI.h>
#include <Render/RenderGraph.h>
#include <Render/Pipeline.h>
#include <Render/Shader.h>

class RenderGraphTestCS : public Render::GlobalShader {
public:
    ShaderInfo(
        "RenderGraphTestCS",
        "/Engine/Shader/Test/RenderGraphTest.esl",
        "CSMain",
        Render::ShaderStage::S_COMPUTE);

    VariantSet();
    DefaultVariantFilter
};
RegisterGlobalShader(RenderGraphTestCS);

class TestComputePass : public Render::RGComputePass {
protected:
    void Setup(Render::RGComputePassBuilder& builder) override
    {
        RHI::Device& device = builder.GetDevice();

        RenderGraphTestCS::VariantSet variantSet;
        Render::ShaderInstance testCS = Render::GlobalShaderMap<RenderGraphTestCS>::Get(device).GetShaderInstance(variantSet);
        Render::ComputePipelineShaderSet shaders = { testCS };

        Render::ComputePipelineDesc pipelineDesc;
        pipelineDesc.shaders = shaders;
        Render::ComputePipeline* pipeline = Render::PipelineCache::Get(device).GetPipeline(pipelineDesc);

        Render::RGComputePassDesc passDesc;
        passDesc.pipeline = pipeline->GetRHI();

        builder.SetPassDesc(passDesc);
        builder.SetAsyncCompute(false);

        uniformBuffer = builder.Create<Render::RGBuffer>("TestUniformBuffer", Render::RGBufferDesc::Create(512, RHI::BufferUsageBits::UNIFORM));
        outputTexture = builder.Create<Render::RGTexture>("TestOutputTexture", Render::RGTextureDesc::Create2D(1024, 1024, RHI::PixelFormat::BGRA8_UNORM, RHI::TextureUsageBits::STORAGE_BINDING));

        // TODO bind group & auto mark read write
    }

    void Execute(RHI::ComputePassCommandEncoder& encoder) override
    {
        // TODO
        encoder.Dispatch(8, 8, 1);
    }

private:
    Render::RGBuffer* uniformBuffer;
    Render::RGTexture* outputTexture;
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
    RHI::UniqueRef<RHI::Device> device;
};

TEST_F(RenderGraphTest, BasicTest)
{
    RHI::UniqueRef<RHI::Fence> mainFence = device->CreateFence();

    Render::RenderGraph renderGraph(*device);
    renderGraph.Setup();
    renderGraph.Compile();
    renderGraph.Execute(mainFence.Get(), nullptr);
    mainFence->Wait();
}

TEST_F(RenderGraphTest, ComputePassTest)
{
    // TODO
}
