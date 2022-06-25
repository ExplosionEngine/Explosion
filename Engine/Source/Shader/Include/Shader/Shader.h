//
// Created by johnk on 2022/6/24.
//

#pragma once

#include <unordered_map>

#include <Shader/Parameter.h>

namespace Shader {
    template <typename VertexFactory, typename ParameterSet, typename VariantSet>
    class Shader {
    public:
        using VertexFactoryType = VertexFactory;
        using ParameterSetType = ParameterSet;
        using VariantSetType = VariantSet;

        ~Shader() = default;

    protected:
        Shader() = default;
    };

    class ShaderRegistry {
    public:
        static ShaderRegistry& Get()
        {
            static ShaderRegistry instance;
            return instance;
        }

        template <typename T>
        void RegisterShader()
        {
            // TODO
        }

        ~ShaderRegistry() = default;

    private:
        ShaderRegistry() = default;
    };
}
