//
// Created by Zach Lee on 2022/10/26.
//

#include <Metal/ShaderModule.h>
#include <Metal/Device.h>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_msl.hpp>

namespace RHI::Metal {

    MTLShaderModule::MTLShaderModule(MTLDevice &dev, const ShaderModuleCreateInfo* createInfo)
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

    void MTLShaderModule::CreateNativeShaderLibrary(const ShaderModuleCreateInfo* createInfo)
    {
        spirv_cross::CompilerMSL compiler(static_cast<const uint32_t*>(createInfo->byteCode), createInfo->size / sizeof(uint32_t));
        spirv_cross::CompilerMSL::Options options;
        options.platform = spirv_cross::CompilerMSL::Options::Platform::macOS;
        options.enable_decoration_binding = true;
        options.pad_fragment_output_components = true;
        compiler.set_msl_options(options);

        std::string source = compiler.compile();
        NSString* nsSource = [[NSString alloc] initWithCString:source.data() encoding:NSASCIIStringEncoding];

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
