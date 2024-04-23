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

// TODO someday macos can build this too
#if PLATFORM_WINDOWS
    static void BuildHlslReflectionData(ComPtr<ID3D12ShaderReflection>& shaderReflection, ShaderReflectionData& result)
    {
        D3D12_SHADER_DESC shaderDesc;
        shaderReflection->GetDesc(&shaderDesc);

        // TODO
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

    static void ConvertSprivToMetalByteCode(
        const ShaderCompileOptions& options,
        ShaderCompileOutput& output)
    {
        spirv_cross::CompilerMSL compiler(reinterpret_cast<const uint32_t*>(output.byteCode.data()), output.byteCode.size() / sizeof(uint32_t));
        spirv_cross::CompilerMSL::Options mslOptions;
        mslOptions.platform = spirv_cross::CompilerMSL::Options::Platform::macOS;
        mslOptions.enable_decoration_binding = true;
        mslOptions.pad_fragment_output_components = true;
        compiler.set_msl_options(mslOptions);

        std::string source = compiler.compile();
        output.byteCode.resize(source.length() + 1, 0);
        memcpy(output.byteCode.data(), source.c_str(), source.length());
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
            if (!output.success || options.byteCodeType != ShaderByteCodeType::mbc) {
                return output;
            }
            ConvertSprivToMetalByteCode(options, output);
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
            std::vector<std::tuple<ShaderTypeKey, VariantKey, std::future<ShaderCompileOutput>>> compileOutputs;
            for (auto* shaderType : shaderTypes) {
                auto typeKey = shaderType->GetKey();
                auto stage = shaderType->GetStage();
                const auto& entryPoint = shaderType->GetEntryPoint();
                const auto& code = shaderType->GetCode();

                const auto& variants = shaderType->GetVariants();
                for (const auto variantKey : variants) {
                    ShaderCompileInput input {};
                    input.source = code;
                    input.entryPoint = entryPoint;
                    input.stage = stage;
                    input.definitions = shaderType->GetDefinitions(variantKey);

                    compileOutputs.emplace_back(std::make_tuple(typeKey, variantKey, ShaderCompiler::Get().Compile(input, options)));
                }
            }

            ShaderTypeCompileResult result;
            for (auto& compileOutput : compileOutputs) {
                auto output = std::get<2>(compileOutput).get();
                if (output.success) {
                    // TODO
                } else {
                    // TODO
                }
            }
            return result;
        }, inShaderTypes, inOptions);
    }

    std::future<ShaderTypeCompileResult> ShaderTypeCompiler::CompileGlobalShaderTypes(const ShaderCompileOptions& inOptions)
    {
        return Compile(GlobalShaderRegistry::Get().GetShaderTypes(), inOptions);
    }
}
