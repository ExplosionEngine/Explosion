//
// Created by johnk on 2022/6/25.
//

#pragma once

#include <sstream>
#include <vector>
#include <utility>
#include <string_view>

#include <meta/factory.hpp>

#include <Common/Platform.h>
#include <Common/Hash.h>
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
        std::vector<uint8_t> CompileEngineShader(const Common::TargetPlatform& targetPlatform, const typename T::VariantSet& variantSet)
        {
            ShaderCompileInfo shaderCompileInfo;
            shaderCompileInfo.targetPlatform = targetPlatform;
            shaderCompileInfo.source = EngineShaderCode<T>::GetFullCode();
            shaderCompileInfo.entryPoint = T::entryPoint;
            shaderCompileInfo.shaderStage = T::stage;
            shaderCompileInfo.definitions = EngineShaderVariants<T>::GetDefinitions(variantSet);
            return CompileShader(shaderCompileInfo);
        }

    private:
        template <typename T>
        struct EngineShaderCode {
        public:
            static std::string GetFullCode()
            {
                static const std::string result = []() -> std::string {
                    std::stringstream codeStream;
                    codeStream << GetVertexFactoryCode();
                    codeStream << GetParameterSetCode();
                    codeStream << GetMainBodyCode();
                    return codeStream.str();
                }();
                return result;
            }

            static uint64_t GetCodeHash()
            {
                static const uint64_t result = []() -> uint64_t {
                    std::string fullCode = GetFullCode();
                    return Common::HashUtils::CityHash(fullCode.data(), fullCode.size());
                }();
                return result;
            }

        private:
            static std::string GetVertexFactoryCode()
            {
                static const std::string result = []() -> std::string {
                    // TODO
                }();
                return result;
            }

            static std::string GetParameterSetCode()
            {
                static const std::string result = []() -> std::string {
                    // TODO
                }();
                return result;
            }

            static std::string GetMainBodyCode()
            {
                static const std::string result = []() -> std::string {
                    // TODO
                }();
                return result;
            }
        };

        template <typename T>
        struct EngineShaderVariants {
        public:
            static std::vector<std::pair<std::string, std::string>> GetDefinitions(const typename T::VariantSet& variantSet)
            {
                std::vector<std::pair<std::string, std::string>> result;
                meta::resolve<typename T::VariantSet>().template data([&result, &variantSet](meta::data data) -> void {
                    std::hash<std::string_view> hash {};
                    auto macroProp = data.template prop(hash("Macro"));
                    if (!macroProp) {
                        return;
                    }
                    result.template emplace_back(std::make_pair(
                        macroProp.value().template cast<std::string_view>(),
                        std::to_string(data.get(variantSet, 0).template cast<int32_t>())
                    ));
                });
                return result;
            }
        };

        struct ShaderCompileInfo {
            Common::TargetPlatform targetPlatform;
            std::string source;
            std::string entryPoint;
            RHI::ShaderStageBits shaderStage;
            std::vector<std::pair<std::string, std::string>> definitions;
        };

        static std::vector<uint8_t> CompileShader(const ShaderCompileInfo& shaderCompileInfo);

        ShaderCompiler() = default;
    };
}
