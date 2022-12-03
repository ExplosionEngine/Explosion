//
// Created by Zach Lee on 2022/10/28.
//

#pragma once

#include <RHI/PipelineLayout.h>
#import <Metal/Metal.h>

namespace RHI::Metal {
    class MTLDevice;

    class MTLPipelineLayout : public PipelineLayout {
    public:
        NON_COPYABLE(MTLPipelineLayout)
        MTLPipelineLayout(MTLDevice& dev, const PipelineLayoutCreateInfo* createInfo);
        ~MTLPipelineLayout() override;
        void Destroy() override;

    private:

        MTLDevice& device;
    };

}
