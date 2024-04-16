//
// Created by johnk on 19/2/2022.
//

#include <RHI/ShaderModule.h>

namespace RHI {
    ShaderModuleCreateInfo::ShaderModuleCreateInfo(const std::string& inEntryPoint, const void* inByteCode, size_t inSize)
        : entryPoint(inEntryPoint)
        , byteCode(inByteCode)
        , size(inSize)
    {
    }

    ShaderModuleCreateInfo::ShaderModuleCreateInfo(const std::string& inEntryPoint, const std::vector<uint8_t>& inByteCode)
        : entryPoint(inEntryPoint)
        , byteCode(inByteCode.data())
        , size(inByteCode.size())
    {
    }

    ShaderModuleCreateInfo& ShaderModuleCreateInfo::SetByteCode(const void* inByteCode)
    {
        byteCode = inByteCode;
        return *this;
    }

    ShaderModuleCreateInfo& ShaderModuleCreateInfo::SetSize(size_t inSize)
    {
        size = inSize;
        return *this;
    }

    ShaderModule::ShaderModule(const ShaderModuleCreateInfo& createInfo) {}

    ShaderModule::~ShaderModule() = default;
}
