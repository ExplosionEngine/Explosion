//
// Created by Zach Lee on 2022/10/27.
//

#pragma once

#include <RHI/Pipeline.h>
#import <Metal/Metal.h>

namespace RHI::Metal {
    class MTLDevice;

    class MTLGraphicsPipeline : public GraphicsPipeline {
    public:
        NON_COPYABLE(MTLGraphicsPipeline)
        MTLGraphicsPipeline(MTLDevice& device, const GraphicsPipelineCreateInfo& createInfo);
        ~MTLGraphicsPipeline() override;
        void Destroy() override;

        id<MTLRenderPipelineState> GetNativePipeline() const;
    private:
        void CreateNativeGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo);
        id<MTLFunction> CreateFunction(ShaderStageBits stage, ShaderModule *module);

        MTLDevice& device;
        id<MTLRenderPipelineState> pipeline;
    };

}
