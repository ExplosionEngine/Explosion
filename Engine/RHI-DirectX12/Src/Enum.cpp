//
// Created by johnk on 5/1/2022.
//

#include <RHI/DirectX12/Enum.h>

namespace RHI::DirectX12 {
    std::string GetShaderStageTargetString(ShaderStage stage)
    {
        static std::unordered_map<ShaderStage, std::string> MAP = {
            { ShaderStage::VERTEX,   "vs_5_0" },
            { ShaderStage::FRAGMENT, "ps_5_0" }
        };
        auto iter = MAP.find(stage);
        if (iter == MAP.end()) {
            throw DX12Exception("found no suitable stage target string");
        }
        return iter->second;
    }
}
