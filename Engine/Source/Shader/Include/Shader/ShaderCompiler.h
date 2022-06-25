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

        template <typename T>
        void EnqueueEngineShaderCompileTask(T&& shader)
        {
            // TODO
        }

        bool AllTasksDown()
        {
            // TODO
            return false;
        }

        uint32_t TasksRemains()
        {
            // TODO
            return 0;
        }

        void WaitAllTasks()
        {
            // TODO
        }

    private:
        ShaderCompiler() = default;
    };
}
