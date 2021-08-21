//
// Created by LiZhen on 2021/7/10.
//

#ifndef EXPLOSION_SHADER_H
#define EXPLOSION_SHADER_H

#include <vector>
#include <unordered_map>
#include <Common/Logger.h>

namespace Explosion::Builder {

#define LOGE(str, ...)                 \
    char buffer[128];                  \
    sprintf(buffer, str, __VA_ARGS__); \
    Explosion::Logger::Error(buffer);

    using SpirvBin = std::vector<uint32_t>;

    struct ShaderParam {
        uint32_t binding;
        std::string name;
    };

    struct UniformAttribute {
        uint32_t offset;
    };

    struct UniformBlockParam : ShaderParam {
        uint32_t size;
    };

    struct BufferBlockParam : ShaderParam {
    };

    struct DescriptorSet {
        std::vector<BufferBlockParam> bufferBlock;
        std::vector<UniformBlockParam> uniformBlock;
        std::unordered_map<std::string, UniformAttribute> uniformMap;
    };

    struct ShaderReflection {
//        std::vector<ShaderInput> attribute;
        std::unordered_map<uint32_t, DescriptorSet> descriptorSets;
    };

    struct Shader {
        SpirvBin bin;
        ShaderReflection reflection;
    };
}
#endif // EXPLOSION_SHADER_H
