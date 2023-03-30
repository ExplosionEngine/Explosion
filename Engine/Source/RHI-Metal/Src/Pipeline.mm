//
// Created by Zach Lee on 2022/10/27.
//

#include <Metal/Pipeline.h>
#include <Metal/Device.h>
#include <Metal/ShaderModule.h>
#include <Metal/Common.h>
#include <Common/Debug.h>

namespace RHI::Metal {
    static const char* GetShaderEntry(ShaderStageBits stage)
    {
        static std::unordered_map<ShaderStageBits, const char*> ENTRY_MAP = {
            {ShaderStageBits::S_VERTEX, "VSMain"},
            {ShaderStageBits::S_PIXEL, "FSMain"}
        };
        auto iter = ENTRY_MAP.find(stage);
        Assert(iter != ENTRY_MAP.end() && "invalid shader stage");
        return iter->second;
    }


    MTLGraphicsPipeline::MTLGraphicsPipeline(MTLDevice& dev, const GraphicsPipelineCreateInfo& createInfo)
        : GraphicsPipeline(createInfo), device(dev)
    {
        CreateNativeGraphicsPipeline(createInfo);
    }

    MTLGraphicsPipeline::~MTLGraphicsPipeline()
    {
        [pipeline release];
    }

    void MTLGraphicsPipeline::Destroy()
    {
        delete this;
    }

    id<MTLRenderPipelineState> MTLGraphicsPipeline::GetNativePipeline() const
    {
        return pipeline;
    }

    void MTLGraphicsPipeline::CreateNativeGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo)
    {
        MTLRenderPipelineDescriptor *pipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];

        MTLVertexDescriptor* vertexDesc = [[MTLVertexDescriptor alloc] init];
        uint32_t attributeIndex = 0;
        for (uint32_t i = 0; i < createInfo.vertexState.bufferLayoutNum; ++i) {
            const VertexBufferLayout &vbLayout = createInfo.vertexState.bufferLayouts[i];
            vertexDesc.layouts[i].stepFunction = vbLayout.stepMode == VertexStepMode::PER_VERTEX ? MTLVertexStepFunctionPerVertex : MTLVertexStepFunctionPerInstance;
            vertexDesc.layouts[i].stepRate = 1;
            vertexDesc.layouts[i].stride = vbLayout.stride;

            for (uint32_t j = 0; j < vbLayout.attributeNum; ++j) {
                const VertexAttribute &attribute = vbLayout.attributes[j];
                vertexDesc.attributes[attributeIndex].format = MTLEnumCast<VertexFormat, MTLVertexFormat>(attribute.format);
                vertexDesc.attributes[attributeIndex].offset = attribute.offset;
                vertexDesc.attributes[attributeIndex].bufferIndex = i;
                ++attributeIndex;
            }
        }

        pipelineDesc.label = @"Graphics Pipeline";
        pipelineDesc.vertexFunction = CreateFunction(ShaderStageBits::S_VERTEX, createInfo.vertexShader);
        pipelineDesc.fragmentFunction = CreateFunction(ShaderStageBits::S_PIXEL, createInfo.pixelShader);
        pipelineDesc.vertexDescriptor = vertexDesc;

        for (uint32_t i = 0; i < createInfo.fragmentState.colorTargetNum; ++i) {
            const ColorTargetState &colorTargetState = createInfo.fragmentState.colorTargets[i];
            pipelineDesc.colorAttachments[i].pixelFormat = MTLEnumCast<PixelFormat, MTLPixelFormat>(colorTargetState.format);
            pipelineDesc.colorAttachments[i].writeMask = GetColorWriteMask(colorTargetState.writeFlags);
        }

        NSError *error = nil;
        pipeline = [device.GetDevice() newRenderPipelineStateWithDescriptor:pipelineDesc error:&error];
//        Assert(pipeline != nil);

        [pipelineDesc release];
        [vertexDesc release];
    }

    id<MTLFunction> MTLGraphicsPipeline::CreateFunction(ShaderStageBits stage, ShaderModule *module)
    {
        MTLShaderModule *mtlShader = static_cast<MTLShaderModule*>(module);
        NSString* entry = [[NSString alloc] initWithCString : GetShaderEntry(stage) encoding: NSASCIIStringEncoding];
        id<MTLFunction> function = [mtlShader->GetNativeLibrary() newFunctionWithName: entry];
        return function;
    }
}
