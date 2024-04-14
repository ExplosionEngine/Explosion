//
// Created by johnk on 2023/3/22.
//

#include <gtest/gtest.h>

#include <Rendering/RenderGraph.h>
#include <Rendering/RenderingCache.h>
#include <Render/Shader.h>

using namespace Render;
using namespace Rendering;

class RGTestCS : public Render::GlobalShader {
public:
    ShaderInfo(
        "RGTestCS",
        "/Engine/Shader/Test/RGTest.esl",
        "CSMain",
        Render::ShaderStage::sCompute);

    BoolShaderVariantField(TestBoolVariant, "TEST_BOOL");
    RangedIntShaderVariantField(TestRangedIntVariant, "TEST_RANGED_INT", 0, 3);
    VariantSet(TestBoolVariant, TestRangedIntVariant);

    DefaultVariantFilter
};
RegisterGlobalShader(RGTestCS)

struct RenderGraphTest : public testing::Test {
    void SetUp() override
    {
        instance = RHI::Instance::GetByType(RHI::RHIType::dummy);

        device = instance->GetGpu(0)->RequestDevice(
            RHI::DeviceCreateInfo()
                .AddQueueRequest(RHI::QueueRequestInfo(RHI::QueueType::graphics, 1)));
    }

    void TearDown() override {}

    RHI::Instance* instance;
    Common::UniqueRef<RHI::Device> device;
};

//TEST_F(RenderGraphTest, BasicTest)
//{
//    RGTestCS::VariantSet variantSet;
//    variantSet.Set<RGTestCS::TestBoolVariant>(true);
//    variantSet.Set<RGTestCS::TestRangedIntVariant>(1);
//
//    ComputePipelineStateDesc psoDesc;
//    psoDesc.shaders.computeShader = GlobalShaderMap<RGTestCS>::Get(*device).GetShaderInstance(variantSet);
//    ComputePipelineState* pso = PipelineCache::Get(*device).GetOrCreate(psoDesc);
//
//    RGBuilder builder(*device);
//
//    RGBufferRef uniformBuffer = builder.CreateBuffer(RGBufferDesc::Create()
//        .Size(1024)
//        .Usages(RHI::BufferUsageBits::uniform | RHI::BufferUsageBits::mapRead)
//        .InitialState(RHI::BufferState::shaderReadOnly)
//        .DebugName("uniformBuffer"));
//
//    RGBufferRef outputBuffer = builder.CreateBuffer(RGBufferDesc::Create()
//        .Size(1920 * 1080)
//        .Usages(RHI::BufferUsageBits::storage)
//        .InitialState(RHI::BufferState::storage)
//        .DebugName("outputBuffer"));
//    outputBuffer->MaskAsUsed();
//
//    RGBindGroupRef mainBindGroup = builder.AllocateBindGroup(RGBindGroupDesc::Create(pso->GetBindGroupLayout(0))
//        .UniformBuffer("uniformBuffer", builder.CreateBufferView(
//            uniformBuffer,
//            RGBufferViewDesc::CreateForUniform()
//                .SetSize(1024)
//                .SetOffset(0)))
//        .StorageBuffer("outputBuffer", builder.CreateBufferView(
//            outputBuffer,RGBufferViewDesc::CreateForUniform()
//                .SetSize(1024)
//                .SetOffset(0))));
//
//    builder.AddComputePass("TestComputePass", { mainBindGroup }, [pso, mainBindGroup](RHI::ComputePassCommandRecorder& encoder) -> void {
//        encoder.SetPipeline(pso->GetRHI());
//        encoder.SetBindGroup(0, mainBindGroup->GetRHI());
//        encoder.Dispatch(1920 / 8, 1080 / 8, 1);
//    });
//
//    Common::UniqueRef<RHI::Fence> mainFence = device->CreateFence(false);
//    RGFencePack fencePack { mainFence.Get() };
//    builder.Execute(fencePack);
//    mainFence->Wait();
//}
