//
// Created by Zach Lee on 2022/10/26.
//

#include <Metal/ShaderModule.h>
#include <Metal/Device.h>

namespace RHI::Metal {

    MTLShaderModule::MTLShaderModule(MTLDevice &dev, const ShaderModuleCreateInfo& createInfo)
        : ShaderModule(createInfo), device(dev)
    {
        CreateNativeShaderLibrary(createInfo);
    }

    MTLShaderModule::~MTLShaderModule()
    {
        [library release];
    }

    void MTLShaderModule::Destroy()
    {
        delete this;
    }

    id<MTLLibrary> MTLShaderModule::GetNativeLibrary() const
    {
        return library;
    }

    void MTLShaderModule::CreateNativeShaderLibrary(const ShaderModuleCreateInfo& createInfo)
    {
        NSString* nsSource = [[NSString alloc] initWithCString:static_cast<const char*>(createInfo.byteCode) encoding:NSASCIIStringEncoding];

        MTLCompileOptions* mtlOptions = [MTLCompileOptions alloc];
        mtlOptions.fastMathEnabled = YES;
        mtlOptions.languageVersion = MTLLanguageVersion2_2;

        NSError* error = nil;
        library = [device.GetDevice() newLibraryWithSource:nsSource
                                                   options:mtlOptions
                                                     error:&error];

        [nsSource release];
        [mtlOptions release];
    }

}
