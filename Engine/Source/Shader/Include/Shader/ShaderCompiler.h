//
// Created by johnk on 2022/6/25.
//

#pragma once

#include <Shader/Shader.h>

namespace Shader {
    class ShaderCompiler {
    public:
        static ShaderCompiler& Get()
        {
            static ShaderCompiler instance;
            return instance;
        }

        ~ShaderCompiler() = default;

        template <typename T>
        void CompileEngineShader()
        {
            // TODO using single thread to compile now, wille replace it with multi thread / multi process in the future
            EngineShaderMap<T>::Get().TraverseVariant([](const auto& variantSet, auto& shader) -> void {
                // TODO
            });
        }

    private:
        ShaderCompiler() = default;
    };
}
