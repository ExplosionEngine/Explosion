//
// Created by johnk on 2022/7/16.
//

#include <unordered_map>
#include <tuple>
#include <utility>

#if PLATFORM_WINDOWS
#include <windows.h>
#include <dxc/d3d12shader.h>
#undef min
#undef max
#else
#define __EMULATE_UUID 1
#endif
#include <dxc/dxcapi.h>

#if PLATFORM_WINDOWS
#include <wrl/client.h>
using namespace Microsoft::WRL;
#define ComPtrGet(name) name.Get()
#else
// defined in dxc WinAdapter.h
#define ComPtr CComPtr
#define ComPtrGet(name) name
#endif

#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_msl.hpp>

#include <Render/ShaderCompiler.h>
#include <Common/Debug.h>
#include <Common/String.h>

namespace Render {
    static RHI::BindingType GetRHIBindingType(const D3D_SHADER_INPUT_TYPE type)
    {
        static const std::unordered_map<D3D_SHADER_INPUT_TYPE, RHI::BindingType> map = {
            { D3D_SIT_CBUFFER, RHI::BindingType::uniformBuffer },
            { D3D_SIT_TEXTURE, RHI::BindingType::texture },
            { D3D_SIT_SAMPLER, RHI::BindingType::sampler },
            { D3D_SIT_UAV_RWTYPED, RHI::BindingType::storageTexture },
            { D3D_SIT_STRUCTURED, RHI::BindingType::storageBuffer },
            { D3D_SIT_UAV_RWSTRUCTURED, RHI::BindingType::storageBuffer }
        };
        return map.at(type);
    }

    static RHI::HlslBindingRangeType GetRHIHlslBindingRangeType(const D3D_SHADER_INPUT_TYPE type)
    {
        static const std::unordered_map<D3D_SHADER_INPUT_TYPE, RHI::HlslBindingRangeType> map = {
            { D3D_SIT_CBUFFER, RHI::HlslBindingRangeType::constantBuffer },
            { D3D_SIT_TEXTURE, RHI::HlslBindingRangeType::texture },
            { D3D_SIT_SAMPLER, RHI::HlslBindingRangeType::sampler },
            { D3D_SIT_UAV_RWTYPED, RHI::HlslBindingRangeType::unorderedAccess },
            { D3D_SIT_STRUCTURED, RHI::HlslBindingRangeType::unorderedAccess },
            { D3D_SIT_UAV_RWSTRUCTURED, RHI::HlslBindingRangeType::unorderedAccess }
        };
        return map.at(type);
    }

    static std::wstring GetDXCTargetProfile(RHI::ShaderStageBits stage)
    {
        static const std::unordered_map<RHI::ShaderStageBits, std::wstring> map = {
            { RHI::ShaderStageBits::sVertex, L"vs" },
            { RHI::ShaderStageBits::sPixel, L"ps" },
            // TODO
        };
        auto iter = map.find(stage);
        Assert(iter != map.end());
        return iter->second + L"_6_2";
    }

    static std::vector<LPCWSTR> GetDXCBaseArguments(const ShaderCompileOptions& options)
    {
        static std::vector<LPCWSTR> basicArguments = {
            DXC_ARG_WARNINGS_ARE_ERRORS,
            DXC_ARG_PACK_MATRIX_ROW_MAJOR
        };

        std::vector<LPCWSTR> result = basicArguments;
        if (options.withDebugInfo) {
            result.emplace_back(L"-Qembed_debug");
            result.emplace_back(DXC_ARG_DEBUG);
        }
        if (options.byteCodeType != ShaderByteCodeType::dxil) {
            result.emplace_back(L"-spirv");
        }
        return result;
    }

    static std::vector<std::wstring> GetEntryPointArguments(const ShaderCompileInput& input)
    {
        return {
            L"-E",
            Common::StringUtils::ToWideString(input.entryPoint)
        };
    }

    static std::vector<std::wstring> GetTargetProfileArguments(const ShaderCompileInput& input)
    {
        return {
            L"-T",
            GetDXCTargetProfile(input.stage)
        };
    }

    static std::vector<std::wstring> GetIncludePathArguments(const ShaderCompileOptions& options)
    {
        std::vector<std::wstring> result;
        for (const auto& includePath : options.includePaths) {
            result.emplace_back(L"-I");
            result.emplace_back(Common::StringUtils::ToWideString(includePath));
        }
        return result;
    }

    static std::vector<std::wstring> GetInternalPredefinition(const ShaderCompileInput& input, const ShaderCompileOptions& options)
    {
        std::vector<std::wstring> result { L"-D" };
        auto def = options.byteCodeType == Render::ShaderByteCodeType::spirv ? std::wstring{L"VULKAN=1"} : std::wstring{L"VULKAN=0"};
        result.emplace_back(def);

        // TODO shader stage definitions
        return result;
    }

    static std::vector<std::wstring> GetDefinitionArguments(const ShaderCompileInput& input, const ShaderCompileOptions& options)
    {
        std::vector<std::wstring> result;

        auto preDef = GetInternalPredefinition(input, options);
        result.insert(result.end(), preDef.begin(), preDef.end());

        for (const auto& definition : input.definitions) {
            result.emplace_back(L"-D");
            result.emplace_back(Common::StringUtils::ToWideString(definition));
        }
        return result;
    }

    static void FillArguments(std::vector<LPCWSTR>& result, const std::vector<std::wstring>& arguments)
    {
        for (const auto& argument : arguments) {
            result.emplace_back(argument.c_str());
        }
    }

#if PLATFORM_WINDOWS
    static void BuildHlslReflectionData(const ComPtr<ID3D12ShaderReflection>& shaderReflection, ShaderReflectionData& result)
    {
        D3D12_SHADER_DESC shaderDesc;
        Assert(SUCCEEDED(shaderReflection->GetDesc(&shaderDesc)));

        for (auto i = 0; i < shaderDesc.InputParameters; i++) {
            D3D12_SIGNATURE_PARAMETER_DESC desc;
            Assert(SUCCEEDED(shaderReflection->GetInputParameterDesc(i, &desc)));

            std::string finalSemantic = desc.SemanticIndex == 0 ? desc.SemanticName : std::string(desc.SemanticName) + std::to_string(desc.SemanticIndex);
            Assert(!result.vertexBindings.contains(finalSemantic));
            result.vertexBindings.emplace(std::make_pair(finalSemantic, RHI::HlslVertexBinding(desc.SemanticName, desc.SemanticIndex)));
        }

        for (auto i = 0; i < shaderDesc.BoundResources; i++) {
            D3D12_SHADER_INPUT_BIND_DESC desc;
            Assert(SUCCEEDED(shaderReflection->GetResourceBindingDesc(i, &desc)));

            Assert(!result.resourceBindings.contains(desc.Name) && desc.BindCount == 1);
            const RHI::ResourceBinding resourceBinding(GetRHIBindingType(desc.Type), RHI::HlslBinding(GetRHIHlslBindingRangeType(desc.Type), desc.BindPoint));
            const ShaderReflectionData::LayoutAndResourceBinding layoutAndResourceBinding = std::make_pair(desc.Space, resourceBinding);
            result.resourceBindings.emplace(std::make_pair(std::string(desc.Name), layoutAndResourceBinding));
        }
    }
#endif

    static void BuildGlslReflectionData(const spirv_cross::Compiler& compiler, ShaderReflectionData& result)
    {
        // TODO
    }

    static void CompileDxilOrSpriv(
        const ShaderCompileInput& input,
        const ShaderCompileOptions& options,
        ShaderCompileOutput& output)
    {
        ComPtr<IDxcLibrary> library;
        Assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library))));

        ComPtr<IDxcCompiler3> compiler;
        Assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler))));

        ComPtr<IDxcUtils> utils;
        Assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils))));

        ComPtr<IDxcIncludeHandler> includeHandler;
        Assert(SUCCEEDED(utils->CreateDefaultIncludeHandler(&includeHandler)));

        ComPtr<IDxcBlobEncoding> source;
        utils->CreateBlobFromPinned(input.source.c_str(), std::strlen(input.source.c_str()), CP_UTF8, &source);

        std::vector<LPCWSTR> arguments = GetDXCBaseArguments(options);
        auto entryPointArgs = GetEntryPointArguments(input);
        auto targetProfileArgs = GetTargetProfileArguments(input);
        auto includePathArgs = GetIncludePathArguments(options);
        auto definitionArgs = GetDefinitionArguments(input, options);
        FillArguments(arguments, entryPointArgs);
        FillArguments(arguments, targetProfileArgs);
        FillArguments(arguments, includePathArgs);
        FillArguments(arguments, definitionArgs);

        DxcBuffer sourceBuffer;
        sourceBuffer.Ptr = source->GetBufferPointer();
        sourceBuffer.Size = source->GetBufferSize();
        sourceBuffer.Encoding = 0u;

        ComPtr<IDxcResult> result;
        const HRESULT operationResult = compiler->Compile(
            &sourceBuffer,
            arguments.data(),
            arguments.size(),
            ComPtrGet(includeHandler),
            IID_PPV_ARGS(&result));

        ComPtr<IDxcBlobEncoding> errorBlob;
        Assert(SUCCEEDED(result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errorBlob), nullptr)));

        if (FAILED(operationResult) || errorBlob->GetBufferSize() > 0) {
            output.success = false;
            output.errorInfo.resize(errorBlob->GetBufferSize());
            memcpy(output.errorInfo.data(), errorBlob->GetBufferPointer(), errorBlob->GetBufferSize());
            return;
        }

        ComPtr<IDxcBlob> codeBlob;
        Assert(SUCCEEDED(result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&codeBlob), nullptr)));

        output.success = true;
        const auto* codeStart = static_cast<const uint8_t*>(codeBlob->GetBufferPointer());
        const auto* codeEnd = codeStart + codeBlob->GetBufferSize();
        output.byteCode = std::vector<uint8_t>(codeStart, codeEnd);

        if (options.byteCodeType == ShaderByteCodeType::dxil) {
#if PLATFORM_WINDOWS
            ComPtr<IDxcBlob> reflectionBlob;
            Assert(SUCCEEDED(result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&reflectionBlob), nullptr)));

            DxcBuffer reflectionBuffer;
            reflectionBuffer.Ptr = reflectionBlob->GetBufferPointer();
            reflectionBuffer.Size = reflectionBlob->GetBufferSize();
            reflectionBuffer.Encoding = 0u;

            ComPtr<ID3D12ShaderReflection> shaderReflection;
            utils->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(&shaderReflection));
            BuildHlslReflectionData(shaderReflection, output.reflectionData);
#endif
        } else {
            spirv_cross::Compiler sprivCrossCompiler(reinterpret_cast<const uint32_t*>(output.byteCode.data()), output.byteCode.size() * sizeof(uint8_t) / sizeof(uint32_t));
            BuildGlslReflectionData(sprivCrossCompiler, output.reflectionData);
        }
    }
}

namespace Render {
    size_t ShaderTypeAndVariantHashProvider::operator()(const std::pair<ShaderTypeKey, VariantKey>& value) const
    {
        return Common::HashUtils::CityHash(&value, sizeof(std::pair<ShaderTypeKey, VariantKey>));
    }

    ShaderCompiler& ShaderCompiler::Get()
    {
        static ShaderCompiler instance;
        return instance;
    }

    ShaderCompiler::ShaderCompiler() : threadPool("ShaderCompiler", 16)
    {
    }

    ShaderCompiler::~ShaderCompiler() = default;

    std::future<ShaderCompileOutput> ShaderCompiler::Compile(const ShaderCompileInput& inInput, const ShaderCompileOptions& inOptions)
    {
        return threadPool.EmplaceTask([](ShaderCompileInput input, ShaderCompileOptions options) -> ShaderCompileOutput {
            ShaderCompileOutput output;
            CompileDxilOrSpriv(input, options, output);
            return output;
        }, inInput, inOptions);
    }

    ShaderTypeCompiler& ShaderTypeCompiler::Get()
    {
        static ShaderTypeCompiler instance;
        return instance;
    }

    ShaderTypeCompiler::ShaderTypeCompiler()
        : threadPool("ShaderTypeCompiler", 4)
    {
    }

    ShaderTypeCompiler::~ShaderTypeCompiler() = default;

    std::future<ShaderTypeCompileResult> ShaderTypeCompiler::Compile(const std::vector<IShaderType*>& inShaderTypes, const ShaderCompileOptions& inOptions)
    {
        return threadPool.EmplaceTask([](std::vector<IShaderType*> shaderTypes, ShaderCompileOptions options) -> ShaderTypeCompileResult {
            std::unordered_map<ShaderTypeKey, std::unordered_map<VariantKey, std::future<ShaderCompileOutput>>> compileOutputs;
            compileOutputs.reserve(shaderTypes.size());
            for (auto* shaderType : shaderTypes) {
                auto typeKey = shaderType->GetKey();
                auto stage = shaderType->GetStage();
                const auto& entryPoint = shaderType->GetEntryPoint();
                const auto& code = shaderType->GetCode();

                Assert(!compileOutputs.contains(typeKey));
                compileOutputs.emplace(std::make_pair(typeKey, std::unordered_map<VariantKey, std::future<ShaderCompileOutput>> {}));
                auto& variantCompileOutputs = compileOutputs.at(typeKey);

                const auto& variants = shaderType->GetVariants();
                for (const auto& variantKey : variants) {
                    ShaderCompileInput input {};
                    input.source = code;
                    input.entryPoint = entryPoint;
                    input.stage = stage;
                    input.definitions = shaderType->GetDefinitions(variantKey);

                    variantCompileOutputs.emplace(std::make_pair(variantKey, ShaderCompiler::Get().Compile(input, options)));
                }
            }

            ShaderTypeCompileResult result;
            for (auto& compileOutput : compileOutputs) {
                auto typeKey = compileOutput.first;
                auto& variantCompileOutputs = compileOutput.second;

                ShaderArchivePackage archivePackage;
                for (auto& variantCompileOutput : variantCompileOutputs) {
                    auto variantKey = variantCompileOutput.first;
                    auto& compileFuture = variantCompileOutput.second;

                    ShaderCompileOutput output = compileFuture.get();
                    if (output.success) {
                        ShaderArchive archive;
                        archive.byteCode = std::move(output.byteCode);
                        archive.reflectionData = std::move(output.reflectionData);

                        archivePackage.emplace(std::make_pair(variantKey, std::move(archive)));
                    } else {
                        result.errorInfos.emplace(std::make_pair(std::make_pair(typeKey, variantKey), output.errorInfo));
                    }
                }
                ShaderArchiveStorage::Get().UpdateShaderArchivePackage(typeKey, std::move(archivePackage));
            }
            result.success = result.errorInfos.size() == 0;
            return result;
        }, inShaderTypes, inOptions);
    }

    std::future<ShaderTypeCompileResult> ShaderTypeCompiler::CompileGlobalShaderTypes(const ShaderCompileOptions& inOptions)
    {
        return Compile(GlobalShaderRegistry::Get().GetShaderTypes(), inOptions);
    }
}
