//
// Created by johnk on 2022/6/25.
//

#pragma once

#include <sstream>

#include <Common/Hash.h>
#include <Shader/Shader.h>

namespace Shader {
    template <typename VertexFactory>
    std::string_view vertexFactoryCode = []() -> void {
        // TODO
    };

    template <typename ParameterSet>
    std::string_view parameterSetCode = []() -> void {
        // TODO
    }();

    template <typename VariantSet>
    std::string_view variantSetCode = []() -> void {
        // TODO
    };

    template <typename T>
    std::string_view shaderMainCode = []() -> void {
        // TODO
    };
}

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
                std::stringstream shaderCodeStream;
                shaderCodeStream << vertexFactoryCode<typename T::VertexFactory>;
                shaderCodeStream << parameterSetCode<typename T::ParameterSet>;
                shaderCodeStream << variantSetCode<typename T::VariantSet>;

                std::string shaderCode = shaderCodeStream.str();
                uint64_t hashCode = Common::HashUtils::CityHash(shaderCode.data(), shaderCode.size());
                if (IsCachedShaderFound(hashCode)) {
                    // TODO
                } else {
                    // TODO
                }
            });
        }

    private:
        static bool IsCachedShaderFound(uint64_t shaderCodeHash)
        {
            // TODO
            return false;
        }

        ShaderCompiler() = default;
    };
}
