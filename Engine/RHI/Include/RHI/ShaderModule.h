//
// Created by johnk on 5/1/2022.
//

#ifndef EXPLOSION_RHI_SHADER_MODULE_H
#define EXPLOSION_RHI_SHADER_MODULE_H

#include <Common/Utility.h>

namespace RHI {
    class ShaderBinary;

    struct ShaderModuleCreateInfo {
        ShaderBinary* binary;
    };

    class ShaderModule {
    public:
        NON_COPYABLE(ShaderModule)
        virtual ~ShaderModule();

    protected:
        explicit ShaderModule(const ShaderModuleCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_SHADER_MODULE_H
