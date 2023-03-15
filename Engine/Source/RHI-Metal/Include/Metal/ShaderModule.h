//
// Created by Zach Lee on 2022/10/24.
//

#pragma once

#include <RHI/ShaderModule.h>
#import <Metal/Metal.h>

namespace RHI::Metal {
    class MTLDevice;

    class MTLShaderModule : public ShaderModule {
    public:
        NON_COPYABLE(MTLShaderModule)
        MTLShaderModule(MTLDevice &dev, const ShaderModuleCreateInfo* createInfo);
        ~MTLShaderModule() override;

        void Destroy() override;

        id<MTLLibrary> GetNativeLibrary() const;

    private:
        void CreateNativeShaderLibrary(const ShaderModuleCreateInfo* createInfo);

        MTLDevice& device;
        id<MTLLibrary> library;
    };
}
