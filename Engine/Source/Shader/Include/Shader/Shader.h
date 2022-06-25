//
// Created by johnk on 2022/6/24.
//

#pragma once

#include <unordered_map>

#include <Shader/Parameter.h>

namespace Shader {
    class IShader {
    public:
        ~IShader() = default;

        virtual bool IsCompiled() = 0;
        virtual std::vector<uint8_t>& GetByteCode() = 0;

    protected:
        IShader() = default;
    };

    template <typename VertexFactory, typename ParameterSet, typename VariantSet>
    class EngineShader : public IShader {
    public:
        using VertexFactoryType = VertexFactory;
        using ParameterSetType = ParameterSet;
        using VariantSetType = VariantSet;

        ~EngineShader() = default;

        bool IsCompiled() override
        {
            return !byteCode.empty();
        }

        std::vector<uint8_t>& GetByteCode() override
        {
            return byteCode;
        }

        void SetByteCode(std::vector<uint8_t>&& inByteCode)
        {
            byteCode = inByteCode;
        }

    protected:
        EngineShader() = default;

    private:
        std::vector<uint8_t> byteCode;
    };
}
