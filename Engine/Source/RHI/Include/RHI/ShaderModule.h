//
// Created by johnk on 19/2/2022.
//

#pragma once

#include <Common/Utility.h>
#include <string>

namespace RHI {
    struct ShaderModuleCreateInfo {
        const void* byteCode;
        size_t size;
    };

    class ShaderModule {
    public:
        NonCopyable(ShaderModule)
        virtual ~ShaderModule();

        virtual void Destroy() = 0;

    protected:
        explicit ShaderModule(const ShaderModuleCreateInfo& createInfo);
    };
}
