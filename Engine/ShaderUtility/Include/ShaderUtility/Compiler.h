//
// Created by johnk on 13/3/2022.
//

#ifndef EXPLOSION_SHADER_UTILITY_SHADER_COMPILER_H
#define EXPLOSION_SHADER_UTILITY_SHADER_COMPILER_H

#include <cstdint>
#include <string>

#include <ShaderConductor/ShaderConductor.hpp>

#include <Common/Utility.h>
#include <Common/Platform.h>

namespace ShaderUtility {
    enum class CompileStage {
        VERTEX,
        FRAGMENT,
        COMPUTE,
        MAX
    };

    enum class ByteCodeType {
        DXIL,
        SPIR_V,
        MAX
    };

    struct ShaderVersion {
        uint8_t major;
        uint8_t minor;
    };

    struct ShaderMacros {
        std::string name;
        std::string value;
    };

    struct ShaderSourceDesc {
        CompileStage stage;
        std::string fileName;
        std::string entryPoint;
        const ShaderMacros* macros;
        size_t macroNum;
    };

    struct ShaderCompilerOptions {};

    struct ByteCodeOutputDesc {
        ByteCodeType type;
    };

    struct ByteCodeOutput {
        void* byteCode;
        size_t size;
    };

    class Compiler {
    public:
        NON_COPYABLE(Compiler)
        ~Compiler();

        static void CompileHLSL(const ShaderSourceDesc& hlsl, const ShaderCompilerOptions& options, ByteCodeOutputDesc& outputDesc, ByteCodeOutput& output);

    private:
        Compiler();
    };
}

#endif//EXPLOSION_SHADER_UTILITY_SHADER_COMPILER_H
