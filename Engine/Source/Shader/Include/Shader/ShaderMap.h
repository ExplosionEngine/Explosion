//
// Created by johnk on 2022/6/25.
//

#pragma once

#include <memory>
#include <functional>

#include <Shader/Shader.h>

namespace Shader {
    using VariantHash = size_t;

    template <typename T>
    using VariantOp = std::function<void(VariantHash, T&)>;

    template <typename T>
    class EngineShaderMap {
    public:
        static EngineShaderMap& Get()
        {
            static EngineShaderMap<T> instance;
            return instance;
        }

        ~EngineShaderMap() = default;

        IShader* FindVariant(const typename T::VariantSetType& variantSet)
        {
            // TODO
            return nullptr;
        }

        void ForeachVariant(VariantOp<T>&& op)
        {
            for (const auto& iter : shaderVariants) {
                op(iter.first, *iter.second.get());
            }
        }

    private:
        EngineShaderMap()
        {
            EmplaceAllShaderVariants();
        }

        void EmplaceAllShaderVariants()
        {
            // TODO
        }

        std::unordered_map<VariantHash, std::unique_ptr<T>> shaderVariants;
    };

    class ShaderRegistry {
    public:
        static ShaderRegistry& Get()
        {
            static ShaderRegistry instance;
            return instance;
        }

        template <typename T>
        void RegisterEngineShader()
        {
            EngineShaderMap<T>::Get();
            //  TODO dispatch shader compile commands
        }

        ~ShaderRegistry() = default;

    private:
        ShaderRegistry() = default;
    };
}
