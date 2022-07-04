//
// Created by johnk on 2022/6/24.
//

#pragma once

#include <unordered_map>
#include <memory>
#include <functional>
#include <string_view>
#include <sstream>

#include <meta/factory.hpp>

#include <Common/Debug.h>
#include <Common/Hash.h>
#include <Shader/Parameter.h>
#include <Shader/ShaderCompiler.h>

#define RegisterEngineShader(Name) \
    static int _NameRegister = []() -> int { \
        Shader::EngineShaderMap<Name>::Get(); \
        return 0; \
    }(); \

namespace Shader {
    class IShader {
    public:
        ~IShader() = default;

        virtual bool IsCompiled() = 0;
        virtual std::vector<uint8_t>& GetByteCode() = 0;

    protected:
        IShader() = default;
    };

    class EngineShader : public IShader {
    public:
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

namespace Shader {
    using VariantHash = size_t;

    template <typename VariantSet, typename Shader>
    using VariantOp = std::function<void(const VariantSet&, Shader&)>;

    template <typename T>
    class EngineShaderMap {
    public:
        using VariantSet = typename T::VariantSet;

        static EngineShaderMap& Get()
        {
            static EngineShaderMap<T> instance;
            return instance;
        }

        ~EngineShaderMap() = default;

        T& GetVariant(const VariantSet& variantSet)
        {
            auto iter = shaders.template find(GetVariantSetHash(variantSet));
            Assert(iter != shaders.end());
            return *iter->second;
        }

        template <typename VariantOp>
        void TraverseVariant(VariantOp&& op)
        {
            for (auto& shader : shaders) {
                op(shader.first, shader.second);
            }
        }

    private:
        static std::pair<uint8_t, uint8_t> GetShaderMacroRange(meta::data data)
        {
            std::hash<std::string_view> hash {};
            auto minValueProp = data.prop(hash("MinValue"));
            auto maxValueProp = data.prop(hash("MaxValue"));

            auto minValue = minValueProp ? minValueProp.value().cast<uint8_t>() : 0;
            auto maxValue = maxValueProp ? maxValueProp.value().cast<uint8_t>() : 1;
            Assert(minValue <= maxValue);
            return std::make_pair(minValue, maxValue);
        }

        static uint32_t GetVariantNum()
        {
            uint32_t result = 1;
            meta::resolve<VariantSet>().data([&result](meta::data data) -> void {
                auto range = GetShaderMacroRange(data);
                result *= range.second - range.first + 1;
            });
            return result;
        }

        static uint64_t GetVariantSetHash(const VariantSet& variantSet)
        {
            return Common::HashUtils::CityHash(&variantSet, sizeof(variantSet));
        }

        EngineShaderMap()
        {
            EmplaceAllVariants();
        }

        void EmplaceAllVariants()
        {
            shaders.reserve(GetVariantNum());
            shaders.emplace_back(std::make_pair<VariantSet, T>({}, T()));
            meta::resolve<VariantSet>().data([this](meta::data data) -> void {
                auto range = GetShaderMacroRange(data);
                auto currentSize = shaders.size();
                for (auto i = range.first; i < range.second; i++) {
                    for (auto j = 0; j < currentSize; j++) {
                        shaders.push_back(shaders[j]);
                        data.set(shaders.back().first, 0, i);
                    }
                }
            });

            for (auto i = 0; i < shaders.size(); i++) {
                variantHashs[GetVariantSetHash(shaders[i].first)] = i;
            }
        }

        std::vector<std::pair<VariantSet, T>> shaders;
        std::unordered_map<VariantHash, size_t> variantHashs;
    };
}
