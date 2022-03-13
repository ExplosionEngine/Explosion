//
// Created by johnk on 19/2/2022.
//

#ifndef EXPLOSION_RHI_SHADER_MODULE_H
#define EXPLOSION_RHI_SHADER_MODULE_H

#include <Common/Utility.h>
#include <string>

namespace RHI {
    struct ShaderModuleCreateInfo {
        void* byteCode;
    };

    class ShaderModule {
    public:
        NON_COPYABLE(ShaderModule)
        virtual ~ShaderModule();

        virtual void Destroy() = 0;

    protected:
        explicit ShaderModule(const ShaderModuleCreateInfo* createInfo);
    };
}

#endif//EXPLOSION_RHI_SHADER_MODULE_H
