//
// Created by johnk on 2022/6/25.
//

#pragma once

namespace Shader {
    class ShaderCompiler {
    public:
        static ShaderCompiler& Get()
        {
            static ShaderCompiler instance;
            return instance;
        }

        ~ShaderCompiler() = default;

    private:
        ShaderCompiler() = default;
    };
}
