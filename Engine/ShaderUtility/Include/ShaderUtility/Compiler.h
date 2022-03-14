//
// Created by johnk on 13/3/2022.
//

#ifndef EXPLOSION_SHADER_UTILITY_SHADER_COMPILER_H
#define EXPLOSION_SHADER_UTILITY_SHADER_COMPILER_H

#include <cstdint>

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

    struct HLSLDesc {
        std::string code;
        std::string entryPoint;
        CompileStage stage;
    };

    struct ByteCodeDesc {
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

        static void CompileHLSL(const HLSLDesc& hlsl, const ByteCodeDesc& byteCode, ByteCodeOutput& output);

    private:
        Compiler();
    };
}

#endif//EXPLOSION_SHADER_UTILITY_SHADER_COMPILER_H
