//
// Created by johnk on 19/2/2022.
//

#pragma once

#include <string>
#include <vector>

#include <Common/Utility.h>

namespace RHI {
    struct ShaderModuleCreateInfo {
        std::string entryPoint;
        const void* byteCode;
        size_t size;

        explicit ShaderModuleCreateInfo(const std::string& inEntryPoint = "", const void* inByteCode = nullptr, size_t inSize = 0);
        explicit ShaderModuleCreateInfo(const std::string& inEntryPoint = "", const std::vector<uint8_t>& inByteCode = {});

        ShaderModuleCreateInfo& SetByteCode(const void* inByteCode);
        ShaderModuleCreateInfo& SetSize(size_t inSize);
    };

    class ShaderModule {
    public:
        NonCopyable(ShaderModule)
        virtual ~ShaderModule();

        virtual const std::string& GetEntryPoint() = 0;
        virtual void Destroy() = 0;

    protected:
        explicit ShaderModule(const ShaderModuleCreateInfo& createInfo);
    };
}
